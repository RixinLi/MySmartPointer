#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include "SmartPointer.h"

// 实现静态类
/*
class sclass{
public:
    // 重载构造两个构造函数阻止类实例化
    void* operator new(size_t t){
        std::cout<<"cannot create a instance of a static class\n";
        assert(false);
        return malloc(t);
    }
    sclass(){
        std::cout<<"cannot create a instance of a static class\n";
        assert(false);
    }

    // 静态方法
    static void print(){
        std::cout<< "this is a static class\n";
    }
    //static const int cnt{0}; // 普通静态变量必须在创建时是const
};
*/

/*
int cnt = 0;

class C{
public:

    // 必须拥有默认构造 除非你不需要改变任何有关构造的东西
    C(){}

    C& operator=(const C& rth){
        a = rth.a;
        return *this;
    }

    // 拷贝深构造
    C(const C& rth):a(rth.a){
    }

    ~C(){
        cnt++;
        std::cout<<"destruct cnt: "<<cnt
        <<std::endl;
    }

    int a = 0;
};
*/
struct A;
struct B;

// 循环引用测试
struct A{
    SP::SharePtr<B> pb;
};
struct B{
    SP::WeakPtr<A> pa;
};

int main() {
    /*
    SM::SharedPtr<int> p(new int(10));
    if (p.unique()) return 0;
    SM::SharedPtr<int> q;
    q.reset(p);

    if(q == p) std::cout<< "they point to same obj"<<std::endl;

    std::cout<<q.use_count()<<std::endl;
     */


    /*
    std::shared_ptr<int> p(new int(10));
    std::shared_ptr<int> q;

    q = p;
    p.reset();

    if (q.get()!= nullptr) return 1;
     */

    /*
    SM::SharedPtr<int> a(new int(10));
    SM::WeakPtr<int> b(a);
    */

    /*
    std::shared_ptr<int> a(new int(10));
    std::weak_ptr<int>b(a);
    std::shared_ptr<int> c = b.lock();
    std::weak_ptr<int> d(b.lock());
    std::cout<<c.use_count()<<std::endl;
    std::cout<<b.use_count()<<std::endl;
    std::weak_ptr<int> e;
    std::cout<<e.use_count()<<std::endl;
    */

    /*
    SM::SharedPtr<int> a(nullptr);
    //std::cout<<*nullptr<<std::endl;
    SM::WeakPtr<int> b = a;
    b = a;
     */
    /*
    {
        SM::SharedPtr<int> a;
        SM::SharedPtr<int> b(a);
        std::cout<<*a<<std::endl;
    }
     */

    //std::cout<<a.use_count()<<" "<<b.use_count()<<std::endl;

    /*
    std::shared_ptr<int> a(new int(10));
    std::weak_ptr<int> b(a);
    std::weak_ptr<int> c(a);
    a.reset();

    std::cout<<b.use_count()<<" "<<c.use_count()<<std::endl;
    if (b.expired()) return 1;
     */

    /*
        int* a = nullptr;
        SP::SharePtr<std::pair<int,int>> b(new std::pair<int,int>{10,10});
        std::pair<int,int> *c = b.Get();
        std::cout<<"before change int: "<<c->first<<std::endl;
        c->first = 6;
        std::cout<<"after change int: "<<c->first<<std::endl;
   */

    /*
    int *a = new int(10);
    SP::SharePtr<int> b,c;
    b.reset(a);
    //c.reset(a);  // 切记一个野指针不能同时给两个没有任何关系的已经定义的智能指针实例管理
    */

    /*
    SP::SharePtr<int> a;
    SP::WeakPtr<int> b(a);
    if (!b.expired()){
        SP::SharePtr<int> c = b.lock();
    }
    */


    SP::SharePtr<A> _a = SP::ShareMake<A>();
    SP::SharePtr<B> _b = SP::ShareMake<B>();
    _a->pb = _b;
    _b->pa = _a;
    return 0;
}
