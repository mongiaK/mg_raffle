/*================================================================
*
*  文件名称：main.cpp
*  创 建 者: mongia
*  创建日期：2020年08月03日
*
================================================================*/

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <set>
#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

using namespace std;
static std::atomic<bool> g_var1(true);

void thread1(int n) {
    std::cout << "thread1 id:" << std::this_thread::get_id() << std::endl;
    while (g_var1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "thread1 exit" << std::endl;
}

void thread2() {
    bool ret = false;
    std::cout << "thread2 id:" << std::this_thread::get_id() << std::endl;
    g_var1.exchange(false);

    std::cout << "thread2 exit" << std::endl;
}

class String {
   private:
    char* a;
    int size;
    int& type;

   public:
    String(int typ = 1) : a(NULL), size(0), type(typ) {
        std::cout << "constructor" << std::endl;
    }

    String(const String& str, int typ = 1) : type(typ) {
        std::cout << "param constructor" << std::endl;
    }

    ~String() { std::cout << "destructor" << std::endl; }

    void operator=(const String& str) {
        std::cout << "= constructor" << std::endl;
    }
};

String func(String c) {
    String c1 = c;
    return c1;
}

int create_share_memory(int size) {
    int shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0600);
    if (shmid < 0) {
        std::cerr << "create share memory failed." << std::endl;
        return -1;
    }

    return shmid;
}

void create_server() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        std::cerr << "socket error: " << errno << std::endl;
        return;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9282);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");

    bind(fd, (struct sockaddr*)(&addr), sizeof(struct sockaddr));

    listen(fd, 1024);
}

void start_accept() {}

void virtual_fuc_test() {
    class Test {
       public:
        virtual void func(){};
    };

    Test* t = new Test;
}

void abstract_class() {
    class Abstract {
       public:
        virtual void func() = 0;
    };

    class son : public Abstract {
       private:
        son(){};

       public:
        virtual void func() final {}
    };

    class son_son : public son {
        son_son* _son_son;
    };
}

int mid_find(int* vec, int length, int num) {
    if (length <= 0) {
        return 1;
    }

    if (length <= 0) {
        return 1;
    }
    int front = 0;
    int end = length - 1;
    while (front < end) {
        int mid_idx = (end - front) / 2;
        int mid = vec[mid_idx];
        if (mid > num) {
            end = mid_idx - 1;
        } else if (mid < num) {
            front = mid_idx + 1;
        } else {
            return mid_idx;
        }
    }
    if (vec[front] != num && vec[front] > num) {
        return front;
    }
    return length + 1;
}

void vec_test() {
    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    for (int i = vec.size() - 1; i >= 0; i--) {
        std::cout << vec[i] << std::endl;
    }

    char a[] = "abc";
    char b[] = "abc";

    char* c = "abc";
    char* d = "abc";

    std::cout << bool(a == b) << ", " << bool(c == d) << std::endl;
};

int arr_test(int* vec, int length) {
    if (length <= 0) return 0;
    int sum = 0;
    int max = vec[0];
    for (int i = 0; i < length; i++) {
        sum += vec[i];
        if (sum > max) {
            max = sum;
        }
        if (sum <= 0) {
            sum = 0;
        }
    }
    return max;
}

void func() {
    struct S {
        int a;
        char c[8];
    } * p;

    S a[2] = {{1, "abc"}, {2, "bcd"}};

    p = a;
    std::cout << (++p)->a << std::endl;

    for (int i = 0, j = 0; j != 100 && i < 5; i++) {
        scanf("%d", &j);
    }
}

int minOperations(vector<int>& nums) {
    if (nums.size() <= 1) {
        return 0;
    }
    int steps = 0;
    int needAddTo = nums[0];
    for (auto item : nums) {
        if (item > needAddTo) {
            needAddTo = item + 1;
        } else {
            steps += needAddTo - item;
            needAddTo += 1;
        }
    }
    return steps;
}

void rotate(vector<int>& nums, int k) {
    int size = nums.size();
    int right = k % size;
    if (right == 0) {
        return;
    }

    int times = 0;
    int save = nums[0];
    int next = right;
    while (times <= size) {
        int tmp = nums[next];
        nums[next] = save;
        save = tmp;
        next = (next + right) % size;
        ++times;
    }
}
int main(int argc, char* argv[]) {
    vector<int> param = {-1, -100, 1, 99};
    rotate(param, 2);
    //    func();
    //    int a[5] = {1, 2, 4, 4, 5};
    //    int ret = mid_find(a, 5, 4);
    //    std::cout << "ret " << ret << std::endl;
    //
    //    int a1[5] = {-5, -4, -3, -2, -1};
    //    ret = arr_test(a1, 5);
    //    std::cout << "ret " << ret << std::endl;

    //    vec_test();
    //
    //    String s;
    //    String s1;
    //    s1 = func(s);
    //
    //    char* ch = "12312";
    //    String* cs = reinterpret_cast<String*>(ch);
    //
    //    int i = 1;
    //
    //    printf("%d,%d\n", ++i, ++i);  // 3,3
    //    printf("%d,%d\n", ++i, i++);  // 5,3
    //    printf("%d,%d\n", i++, i++);  // 6,5
    //    printf("%d,%d\n", i++, ++i);  // 8,9
    //
    //    int* p[10];
    //
    //    std::cout << "*p[10]: " << sizeof(p) << std::endl;
    //
    //    std::atomic<int> var(4);
    //
    //    std::shared_ptr<int> a(new int[10]);
    //    std::cout << "before: " << a.use_count() << std::endl;
    //
    //    std::shared_ptr<int> b(a);
    //    std::cout << "use a: " << a.use_count() << std::endl;
    //    std::cout << "use b: " << b.use_count() << std::endl;
    //
    //    std::weak_ptr<int> wk(b);
    //    std::cout << "use wk: " << wk.use_count() << std::endl;
    //
    //    int exp_var = 5;
    //    int new_var = 6;
    //
    //    if (var.compare_exchange_weak(exp_var, new_var)) {
    //        std::cout << "exp == var, var = " << var.load() << ", exp = " <<
    //        exp_var
    //                  << std::endl;
    //    } else {
    //        std::cout << "exp != var, var = " << var.load() << ", exp = " <<
    //        exp_var
    //                  << std::endl;
    //    }
    //
    //   char* conp = "abc";
    //   char conp1[4] = "abc";
    //
    //   std::cout << "val: " << (conp == "abcd") << std::endl;
    //    //    int n = 1;
    //    //    std::thread th1(thread1, n);
    //    //    std::thread th2(thread2);
    //    //
    //    //    th1.join();
    //    //    th2.join();
    //    //
    //    //    std::cout << "main thread id:" << std::this_thread::get_id() <<
    //    "
    //    //    exit" << std::endl;
    return 0;
}
