/*================================================================
*
*  文件名称：rbtree.h
*  创 建 者: mongia
*  创建日期：2021年09月27日
*
================================================================*/

#include "lock.h"

#ifdef MG_DEBUG
#include "log.h"
#endif

namespace mg {

enum RBNodeType {
    RBRedNode = 0,
    RBBlackNode = 1,
};

template <class T>
struct RBNode {
    T _val;
    RBNodeType _type;

    RBNode<T>* _parent;
    RBNode<T>* _lchild;
    RBNode<T>* _rchild;
};

template <class T>
class RBTree {
   public:
    RBTree() : _tree(nullptr), _count(0) {}
    ~RBTree() {}

   public:
    RBNode<T>* search(const T& val) {
        RBNode<T>* p = _tree;
        while (p != nullptr) {
            if (p->_val > val) {
                p = p->_lchild;
            } else if (p->_val < val) {
                p = p->_rchild;
            } else {
                return p;
            }
        }

        return nullptr;
    }

    /*
     * return val
     *
     * 0: success
     * 1: error
     * -1: exist
     *
     * */
    int insert(T val) {
        RBNode<T>* locate = create_node(val);
        if (nullptr == locate) {
#ifdef PDEBUG
            plog_error("create node failed");
#endif
            return 0;
        }

        SpinLock _(_lock);
        if (_tree == nullptr) {  // tree is empty
            _tree = locate;
            _tree->_type = RBBlackNode;
            return 1;
        }

        // find insert position
        RBNode<T>* root = _tree;
        RBNode<T>* p = nullptr;
        while (root != nullptr) {
            p = root;
            if (root->_val > val) {
                root = root->_lchild;
            } else if (p->_val < val) {
                root = root->_rchild;
            } else {
#ifdef PDEBUG
                plog_warning("node exist");
#endif
                return -1;
            }
        }

        // insert node
        locate->_parent = p;
        if (locate->_val < p->_val) {
            p->_lchild = locate;
        } else {
            p->_rchild = locate;
        }
        _count++;

        // rebalance tree
        RBNode<T>* u = nullptr;
        RBNode<T>* pp = nullptr;
        while ((p = locate->_parent) &&
               (p->_type == RBRedNode)) {  // insert is red, parent is red too,
                                           // so need to rebalance

            pp = p->_parent;  // parent's parent node, because parent is red, so
                              // pp must be exist
            u = locate == p->_rchild ? pp->_lchild : pp->_rchild;  // uncle node

            if (u && u->_type == RBRedNode) {
                //             PP (Black)                        PP(Red)
                //             /       \                          /  \
                //         P(Red)      U(Red)         =>    P(Black) U(Black)
                //           /                                  /
                //       I(Red)                              I(Red)

                pp->_type = RBRedNode;
                u->_type = RBBlackNode;
                p->_type = RBBlackNode;

                locate = pp;
                continue;
            } else {
                if (p == pp->_lchild) {
                    //            pp(Black)          P(Black)
                    //             /                  /    \
                    //          p(Red)       =>     I(Red) PP(Red)
                    //           /
                    //         I(Red)
                    if (locate == p->_lchild) {
                        p->_type = RBBlackNode;
                        pp->_type = RBRedNode;
                        right_rotate(pp);
                    } else {
                        //      pp(Black)        pp(Black)         I(Black)
                        //       /                 /                /   \
                        //     p(Red)     =>     I(Red)    =>    P(Red) PP(Red)
                        //        \              /
                        //        I(Red)       p(Red)
                        left_rotate(p);
                        locate->_type = RBBlackNode;
                        pp->_type = RBRedNode;
                        right_rotate(pp);
                        break;  // subtree is balance tree
                    }
                } else {
                    //            pp(Black)            P(Black)
                    //               \                  /    \
                    //             p(Red)       =>   PP(Red) I(Red)
                    //                \
                    //              I(Red)
                    if (locate == p->_rchild) {
                        p->_type = RBBlackNode;
                        pp->_type = RBRedNode;
                        left_rotate(pp);
                    } else {
                        //    pp(Black)       pp(Black)         I(Black)
                        //      \                \                /   \
                        //     p(Red)     =>     I(Red)    => PP(Red) p(Red)
                        //      /                  \
                        //    I(Red)              p(Red)
                        right_rotate(p);
                        locate->_type = RBBlackNode;
                        pp->_type = RBRedNode;
                        left_rotate(pp);
                        break;  // sub tree is balance tree
                    }
                }
            }
        }

        _tree->_type = RBBlackNode;
    }

    void del(T& val) {
        SpinLock _(_lock);

        RBNode<T>* d = search(val);  // del node
        RBNode<T>* r = nullptr;      // replace node

        if (d == nullptr) return;  // node not exist

        // find replace node  and replace node into delete position, set replace
        // node is delete
        if (d->_lchild != nullptr && d->_rchild != nullptr) {
            r = find_right_replace_node(d);
            change_node(d, r);
        }

        // if node is red, so the node dont have leaf
        if (d->_type == RBRedNode) {
            delete_leaf_node(d);
            return;
        }

        // delete node is black, and has one leaf, leaf must be red
        if (d->_lchild != nullptr) {
            change_node(d, d->_lchild);
            delete_leaf_node(d);
            return;
        } else if (d->_rchild != nullptr) {
            change_node(d, d->_rchild);
            delete_leaf_node(d);
            return;
        }

        // 思想：
        // 第一步：把兄弟节点边黑
        // 第二步：兄弟节点的子节点都是黑，那么直接将兄弟节点设置为红色，递归处理
        // 第三步：兄弟节点的子节点存在红色节点，那么把这个红色节点借过来，保证黑色节点删除之后黑高不变。

        // delete node is black, dont have leaf
        RBNode<T>* p = d->_parent;
        RBNode<T>* b = nullptr;
        RBNode<T>* node =
            d;  // node
                // 表示该子树的黑高即将减1，要么在node跟其父亲之间加一个黑色节点，要么将node的兄弟节点减一个黑色节点
        while ((node && node->_type == RBBlackNode) && node != _tree) {
            if (p->_lchild == node) {
                b = p->_rchild;
                if (b->_type ==
                    RBRedNode) {  // 执行此if语句并未将node对立侧的黑高-1或+1，只是从新找了一个uncle节点
                    //         P(b)                      b(r) b(b)
                    //     /        \                   /     \                /
                    //     \
                    //  node(b)    b(r)         =>    p(b)     br(b)    =>  p(r)
                    //  br(b)
                    //              /   \            /    \                 /  \
                    //           bl(b) br(b)     node(b)  bl(b)         node(b)
                    //           bl(b)
                    b->_type = RBBlackNode;
                    p->_type = RBRedNode;
                    left_rotate(p);
                    b = p->_rchild;
                }

                // 如果执行了上面的if语句，此刻的b值得是上面的bl，如果bl存在左右子树，那么必然是红色。
                // 所以走到这个条件语句有两种情况，第一种是第一次f循环，bl的左右子树都是空的，第二种情况就是递归到树的更上层的时候，左右都是黑色，只有一个是黑色的是不存在的。不满足红黑树平衡
                if ((b->_lchild == nullptr ||
                     b->_lchild->_type == RBBlackNode) &&
                    (b->_rchild == nullptr ||
                     b->_rchild->_type ==
                         RBBlackNode)) {  // 此if语句将node对立侧黑高减1，所以需要递归到上一级，祖父节点去调整
                    b->_type = RBBlackNode;
                    node = p;
                    p = node->_parent;
                } else {
                    if (b->_rchild->_type ==
                        RBBlackNode) {  // 此if将uncle节点的右孩子变成红色
                        //            p()                     p() p()
                        //          /    \                   /
                        //          \                /     \
                        //        node() b(b)        =>    node()  b(r)      =>
                        //        node()  bl(b)
                        //                /  \                    / \ /   \
                        //            bl(r)  br(b)              bl(b) br(b)
                        //            bll()   b(r)
                        //                                                                    /    \
                        //                                                                  blr()  br(b)
                        b->_lchild->_type = RBBlackNode;
                        b->_type = RBRedNode;
                        right_rotate(b);
                        b = p->_rchild;
                    }
                    //          p()                      p(b) b()
                    //        /     \                   /     \                /
                    //        \
                    //      node()  b(b)          => node()  b()        => p(b)
                    //      br(b)
                    //             /   \                     /  \           / \
                    //            bl()   br(r)             bl() br(b)    node()
                    //            bl()

                    b->_type = p->_type;
                    p->_type = RBBlackNode;
                    b->_rchild = RBBlackNode;
                    left_rotate(p);
                    break;
                }
            } else {
                b = p->_lchild;
                if (b->_type == RBRedNode) {
                    //         P(b)                      p(r) b(b)
                    //     /        \                   /     \                /
                    //     \
                    //   b(r)      node()      =>    b(b)    node(b)    => bl(b)
                    //   p(r)
                    //      /   \                        / \ /  \
                    //bl(b) br(b)                bl(b)  br(b) br(b) node(b)
                    b->_type = RBBlackNode;
                    p->_type = RBRedNode;
                    right_rotate(p);
                    b = p->_lchild;
                }

                if ((b->_lchild == nullptr ||
                     b->_lchild->_type == RBBlackNode) &&
                    (b->_rchild == nullptr ||
                     b->_rchild->_type == RBBlackNode)) {
                    b->_type = RBBlackNode;
                    node = p;
                    p = node->_parent;
                } else {
                    if (b->_lchild->_type ==
                        RBBlackNode) {  // 此if将兄弟节点的右孩子变成红色
                        //           p()
                        //         /     \
                        //       b(b)    node()
                        //      /   \
                        //    bl(b)    br()
                        b->_rchild->_type = RBBlackNode;
                        b->_type = RBRedNode;
                        left_rotate(b);
                        b = p->_lchild;
                    }
                    //          p()                      p(b) b()
                    //        /     \                   /     \                /
                    //        \
                    //      node()  b(b)          => node()  b()        => p(b)
                    //      br(b)
                    //             /   \                     /  \           / \
                    //            bl()   br(r)             bl() br(b)    node()
                    //            bl()

                    b->_type = p->_type;
                    p->_type = RBBlackNode;
                    b->_lchild = RBBlackNode;
                    right_rotate(p);
                    break;
                }
            }
        }
        node->_type = RBBlackNode;

        delete_leaf_node(d);
    }

   private:
    //                  P                         p
    //                /                          /
    //              node                       rchild
    //              /  \                        /  \
    //         lchild  rchild         =>     node   rrchild
    //                  /   \                /  \
    //            rlchild   rrchild      lchild  rlchild
    //
    void left_rotate(RBNode<T>* node) {
        RBNode<T>* p = node->_parent;
        RBNode<T>* rchild = node->_rchild;

        if (rchild->_lchild != nullptr) rchild->_lchild->_parent = node;

        node->_rchild = rchild->_lchild;
        rchild->_parent = p;

        if (p == nullptr) {
            _tree = rchild;
        } else if (p->_lchild == node) {
            p->_lchild = rchild;
        } else if (p->_rchild == node) {
            p->_rchild = rchild;
        }
        rchild->_lchild = node;
        node->_parent = rchild;
    }

    //                  P                         p
    //                /                          /
    //              node                        lchild
    //              /  \                        /  \
    //         lchild  rchild         =>   llchild node
    //         /   \                               /  \
    //   llchild   lrchild                   lrchild  rchild
    //
    void right_rotate(RBNode<T>* node) {
        RBNode<T>* p = node->_parent;
        RBNode<T>* lchild = node->_lchild;

        if (lchild->_rchild != nullptr) lchild->_rchild->_parent = node;

        node->_lchild = lchild->_rchild;
        lchild->_parent = p;

        if (p == nullptr) {
            _tree = lchild;
        } else if (p->_lchild == node) {
            p->_lchild = lchild;
        } else {
            p->_rchild = lchild;
        }

        lchild->_rchild = node;
        node->_parent = lchild;
    }

    RBNode<T>* create_node(T& val) {
        RBNode<T>* node = new RBNode<T>();
        if (node == nullptr) {
            return nullptr;
        }
        node->_type = RBRedNode;
        node->_val = val;
        node->_parent = nullptr;
        node->_lchild = nullptr;
        node->_rchild = nullptr;

        return node;
    }

    void delete_leaf_node(RBNode<T>* leaf) {
        --_count;
        if (leaf == _tree) {
            _tree = nullptr;
            delete leaf;
            return;
        }

        RBNode<T>* p = leaf->_parent;
        if (p->_lchild == leaf) {
            p->_lchild = nullptr;
        } else {
            p->_rchild = nullptr;
        }
        delete leaf;
    }

    // n1 is higher than n2
    void change_node(RBNode<T>* n1, RBNode<T>* n2, bool change_type = true) {
        if (n1 == n2) return;
        RBNode<T>* n1p = n1->_parent;
        RBNode<T>* n2p = n2->_parent;

        RBNode<T>* tmp;

        tmp = n1->_lchild;
        n1->_lchild = n2->_lchild;
        n2->_lchild = tmp;

        tmp = n1->_rchild;
        n1->_rchild = n2->_rchild;
        n2->_rchild = tmp;

        n1->_parent = n2p;
        n2->_parent = n1p;

        if (n1p == nullptr) {
            _tree = n2;
        } else {
            if (n1p->_lchild == n1) {
                n1p->_lchild = n2;
            } else {
                n1p->_rchild = n2;
            }
        }

        if (n2p->_lchild == n2) {
            n2p->_lchild = n1;
        } else {
            n2p->_rchild = n1;
        }

        if (change_type) {
            RBNodeType typ = n1->_type;
            n1->_type = n2->_type;
            n2->_type = typ;
        }
    }

    RBNode<T>* find_right_replace_node(RBNode<T>* n) {
        RBNode<T>* r = n->_rchild;
        while (r->_lchild != nullptr) {
            r = r->_lchild;
        }
        return r;
    }

   private:
    RBNode<T>* _tree;
    uint64_t _count;

    SpinMutex _lock;
};

}  // namespace mg

#ifdef MG_TEST
void rbtree_test() {
    mg::RBTree<int> rb;
    rb.insert(1);
    rb.insert(5);
    rb.insert(8);
    rb.insert(10);

    rb.search(2);
    rb.search(5);

    rb.del(5);
    rb.del(1);

    rb.insert(123);
    rb.insert(12);
    rb.insert(42);
    rb.insert(1);
    rb.insert(1234);
}
#endif
