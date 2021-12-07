/*================================================================
*
*  文件名称：rbtree.h
*  创 建 者: mongia
*  创建日期：2021年09月27日
*
================================================================*/

#include <bits/stdint-uintn.h>
#include <pthread.h>

#include "lock.h"
#include "log.h"

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
    RBTree() : _tree(nullptr) { pthread_spin_init(&_lock, 0); }
    ~RBTree() { pthread_spin_destroy(&_lock); }

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

    RBNode<T>* create_node(T val) {
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

    void insert(T val) {
        SpinLock _(_lock);
        if (_tree == nullptr) {
            _tree = create_node(val);
            if (_tree != nullptr) _tree->_type = RBBlackNode;
            return;
        }

        RBNode<T>* locate = create_node(val);
        if (locate == nullptr) return;

        // insert node and find parent node
        RBNode<T>* root = _tree;
        RBNode<T>* p = nullptr;
        while (root != nullptr) {
            p = root;
            if (root->_val > val) {
                root = root->_lchild;
            } else if (p->_val < val) {
                root = root->_rchild;
            } else {
                root->_val = val;
                return;
            }
        }

        locate->_parent = p;
        if (locate->_val < p->_val) {
            p->_lchild = locate;
        } else {
            p->_rchild = locate;
        }
        _count++;

        // rebalance tree
        RBNode<T>* s = nullptr;
        RBNode<T>* pp = nullptr;
        while (
            (p = locate->_parent) &&
            (p->_type == RBRedNode)) {  // parent is black， break， because
                                        // insert is red，dont need to rebalance
            pp = p->_parent;  // parent's parent node
            s = locate == p->_rchild ? p->_lchild : p->_rchild;  // uncle  node

            //             PP (Black)                        PP(Red)
            //             /       \                          /  \
            //         P(Red)      S(Red)         =>    P(Black) S(Black)
            //           /                                  /
            //       I(Red)                              I(Red)
            if (s && s->_type == RBRedNode) {
                pp->_type = RBRedNode;
                s->_type = RBBlackNode;
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

        RBNode<T>* d = _tree;    // del node
        RBNode<T>* r = nullptr;  // replace node

        while (d != nullptr) {
            if (d->_val > val) {
                d = d->_lchild;
            } else if (d->_val < val) {
                d = d->_rchild;
            } else {
                break;  // find node
            }
        }

        if (d == nullptr) return;  // no this node

        // find replace node
        if (d->_lchild != nullptr && d->_rchild != nullptr) {
            r = find_right_replace_node(d);
        } else if (d->_lchild != nullptr) {
            r = find_left_replace_node(d);
        } else if (d->_rchild != nullptr) {
            r = find_right_replace_node(d);
        }

        if (r == nullptr) {  // delete is tree root
            delete_leaf_node(d);
            return;
        }

#if PDEBUG
        if (r->_lchild != nullptr && r->_rchild != nullptr) {
            plog_error("replace node find error");
            return;
        }
#endif

        // replace is red

        //                      r(Red)
        //               /                 \
        //      rlchild(nil or black)  rrchild(nil or black)
        if (r->_type == RBRedNode) {  // replace node has only one child at most
            change_node(d, r);        // change place with delete node
            // replace has lchild, because replace is red， so the child must be
            // black, so change replace with child
            if (d->_lchild != nullptr) {
#if PDEBUG
                if (d->_lchild->_type == RBRedNode) {
                    plog_error("replace is red, it's child is red too, error happen")
                }
#endif
                change_node(d, d->_lchild, false);
            } else if (d->_rchild != nullptr){
#if PDEBUG
                if (d->_rchild->_type == RBRedNode) {
                    plog_error("replace is red, it's child is red too, error happen")
                }
#endif
                change_node(d, d->_rchild, false);
            }
            delete_leaf_node(d);  // delete node
            return;
        }

        // replace is black

        // replace node has only one child at most
        //                            r(Black)
        //           /                                   \
        //       rlchild(nil or black or red)     rrchild(nil or black or red)

        if (r->_lchild != nullptr) {  
            //                  d(black)
            //                       
            //                r(black)
            //                /
            //              rlchild(red)
            if (r->_lchild->_type == RBRedNode) {
                change_node(d, r, false);

                change_node(d, d->_lchild);
                delete_leaf_node(d);
                return;
            } else {
#if PDEBUG
                plog_error(
                    "replace is black,and it's child is black too, error "
                    "happen");
                return;
#endif
            }
        } else if (r->_rchild != nullptr) {  // replace is black,
            //                  d(black)
            //                       
            //                r(black)
            //                      \
            //                   rrchild(red)
            //
            if (r->_rchild->_type == RBRedNode) {
                change_node(d, r, false);

                change_node(d, d->_rchild);
                delete_leaf_node(d);
                return;
            } else {  
#if PDEBUG
                plog_error(
                    "replace is black, and it's child is black too, error "
                    "happen");
                return;
#endif
            }
        }

        // delete black leaf node
        change_node(d, r);
        RBNode<T>* p = d->_parent;
        RBNode<T>* s = nullptr;
        RBNode<T>* sl = nullptr;
        RBNode<T>* sr = nullptr;
    
        if (d == p->_lchild) {
            s = p->_rchild;
            if (s->_type == RBRedNode) {
                //               p(black)                             p(Red)                            s(black)
                //                /    \                               /   \                             /    \
                //          d(black)  s(red)           =>      d(black)  s(black)              =>     p(red)  sr(black)
                //                    /    \                               /    \                    /    \
                //              sl(black)  sr(black)                  sl(black)  sr(black)     d(black) sl(black)
                //
                s->_type = RBBlackNode;
                p->_type =  RBRedNode;

                left_rotate(p);
                delete_leaf_node(d);
            } else if (s->_type == RBBlackNode) {
                sl = s->_lchild;
                sr = s->_rchild;
                if (sr != nullptr && sr->_type == RBRedNode) {
                    //                p(p color)                                p(black)                                          s(p color)
                    //                 /        \                                 /   \                                           /          \
                    //              d(black)   s(black)             =>     d(black)  s(p color)            =>              p(black)       sr(black)
                    //                              \                                      \                                     /
                    //                              sr(red or nil)                        sr(black)                         d(black)
                    s->_type = p->_type;
                    p->_type = RBBlackNode;
                    sr->_type = RBBlackNode;
                    left_rotate(p);
                    delete_leaf_node(d);
                } else if (sl != nullptr && sl->_type == RBRedNode) {
                    //                p(p color)                                 p(black)                                          sl(p color)
                    //                 /        \                                 /   \                                           /          \
                    //              d(black)   s(black)             =>     d(black)  sl(p color)            =>                    p(black)     s(black)
                    //                          /                                         \                                     /   \
                    //                       sl(red)                                     s (black)                         d(black) sl()
                    right_rotate(s);
                    sl->_type = p->_type;
                    p->_type = RBBlackNode;
                    left_rotate(p);
                    delete_leaf_node(d);
                } else {
                    //                      p(p color)
                    //                       /     \
                    //                  d(black)   s(black)
                    if (p->_type == RBRedNode) {
                        left_rotate(p);
                        delete_leaf_node(d);
                    } else {

                    }
                }
            }
        }
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

    RBNode<T>* find_left_replace_node(RBNode<T>* n) {
        RBNode<T>* r = n->_lchild;
        while (r->_rchild != nullptr) {
            r = r->_rchild;
        }
        return r;
    }

    RBNode<T>* search(T val) {
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

   private:
    RBNode<T>* _tree;
    uint64_t _count;
    pthread_spinlock_t _lock;
};
