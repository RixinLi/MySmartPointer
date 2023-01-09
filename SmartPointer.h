//
// Created by RixinLi on 2023/1/7.
//
// Library of SmartPointer: SharePtr and WeakPtr

#ifndef _SM_SMARTPOINTER_H
#define _SM_SMARTPOINTER_H

#include <iostream>
#include <mutex>
#include <thread>
#include <memory>
#include <atomic>

// SP:自制智能指针类命名空间
namespace SP{

    template<typename T> class SharePtr;
    template<typename T> class WeakPtr;

    // SharePtr 与 WeakPtr 都需要的计数器类

    class Share_Weak_Count{
    public:
        Share_Weak_Count(long sc = 0, long wc = 0):ShareCnt(sc),WeakCnt(wc){}
        std::atomic<long> ShareCnt; // 原子量保证计数器稳定
        std::atomic<long> WeakCnt;
    };

    // SharePtr

    template<typename T>
    class SharePtr{

    public:
        friend class WeakPtr<T>;
        // 构造与析构函数

        /*
        SharePtr():pPtr(nullptr),Count(new Share_Weak_Count()),m_mutex(new std::mutex){
            std::cout<<"use default SharePtr constructor"<<std::endl;
        }

        SharePtr(std::nullptr_t):pPtr(nullptr),Count(new Share_Weak_Count()),m_mutex(new std::mutex){
            std::cout<<"use nullptr_t SharePtr constructor"<<std::endl;
        }*/

        SharePtr(T* nPtr = nullptr){
            // 这里不能直接复制， 必须检查nPtr是否指向nullptr，如果指向nullptr，设置独立的空的SharePtr
            if(nPtr == nullptr){
                pPtr = nullptr,Count = new Share_Weak_Count(),m_mutex = new std::mutex;
            }
            else{
                pPtr = nPtr, Count = new Share_Weak_Count(1), m_mutex = new std::mutex;
            }
            std::cout<<"use original pointer SharePtr constructor"<<std::endl;
        }

        SharePtr(const SharePtr<T>& rPtr)
        {
            // 这里不能直接复制， 必须检查rPtr是否指向nullptr，如果指向nullptr，拷贝复制为一个新的空SharePtr
            if(!rPtr.pPtr){
                pPtr = nullptr,Count = new Share_Weak_Count(),m_mutex = new std::mutex;
            }
            else{
                pPtr = rPtr.pPtr, Count = rPtr.Count, m_mutex = rPtr.m_mutex;
                Count->ShareCnt++;
            }
            std::cout<<"use copy SharePtr constructor"<<std::endl;
        }

        // 用于强转的构造函数
        SharePtr(WeakPtr<T> const& w){
            // 转换前必须保证 w没有过期，如果过期了，重构一个默认的 同上
            if(w.expired()){
                pPtr = nullptr,Count = new Share_Weak_Count(),m_mutex = new std::mutex;
            }else{
                pPtr = w.pPtr, Count = w.Count, m_mutex = w.m_mutex;
                Count->ShareCnt++;
                std::cout<<"use WeakPtr to SharePtr Constructor"<<std::endl;
            }


        }

        ~SharePtr(){
            release();
            std::cout<<"finish SharePtr destruction"<<std::endl;
        }

    public:

        // 符号重载
        SharePtr<T>& operator=(const SharePtr<T>& rPtr){
            if (this != &rPtr){
                if(rPtr.pPtr){ //同上 必须检查极端情况
                    release();
                    pPtr = rPtr.pPtr;
                    Count = rPtr.Count;
                    Count->ShareCnt++;
                }
                else {
                    release();
                    pPtr = nullptr,Count = new Share_Weak_Count(),m_mutex = new std::mutex;
                }
            }
            return *this;
        }

        operator bool(){
            return pPtr != nullptr;
        }

        bool operator==(SharePtr<T>& rPtr){
            return pPtr == rPtr.pPtr;
        }

        T& operator *(){
            if (pPtr) return *pPtr;
            std::cout<<"you try to parse a nullptr pointer"<<std::endl;
            assert(false);
            return nullptr;
        }

        T* operator->(){
            return pPtr;
        }

    public:
        /*成员函数*/

        T* Get(){
            return pPtr;
        }

        // 重置 1: 直接归空
        void reset(){
            std::cout<<"SharePtr reset to nullptr"<<std::endl;
            release();
            pPtr = nullptr,
            Count = new Share_Weak_Count(),
            m_mutex = new std::mutex;
        }

        // 重置 2: 管理另一个野指针
        void reset(T* ptr){
            if (pPtr != ptr){
                if (ptr){
                    release();
                    pPtr = ptr, Count = new Share_Weak_Count(1), m_mutex = new std::mutex;
                    std::cout<<"SharePtr reset to manage another pointer"<<std::endl;
                }else{
                    release();
                    pPtr = nullptr,
                    Count = new Share_Weak_Count(),
                    m_mutex = new std::mutex;
                }
            }
        }

        // 重置 3
        void reset(SharePtr<T>& rPtr){
            if (this != &rPtr){
                std::cout<<"reset to another SharePtr"<<std::endl;
                release();
                pPtr = rPtr.pPtr,
                Count = rPtr.Count,
                m_mutex = rPtr.m_mutex;
                Count->ShareCnt++;
            }
        }

        long use_count() const{
            if(Count) return Count->ShareCnt;
            return 0;
        }

        bool unique() const{
            if(Count) return Count->ShareCnt == 1;
            return false;
        }



    protected:
        void release(){
            bool final = false;
            std::cout<<"releasing "<<Count->ShareCnt<<" SharePtr"<<std::endl;
            m_mutex->lock();
            if (pPtr == nullptr || --(Count->ShareCnt)==0){
                std::cout<< "releasing the original pointer"<<std::endl;
                delete pPtr;
                if (Count->WeakCnt < 1) delete Count;
                final = true;
            }
            m_mutex->unlock();
            if (final) delete m_mutex;
        }

    private:
        T* pPtr; // 原生指针
        Share_Weak_Count* Count; // 计数器类
        std::mutex* m_mutex; // 互斥量 保证析构的线程安全
    };



    /*WeakPtr*/

    template<typename T>
    class WeakPtr{
    public:
        // 互为好朋友
        friend class SharePtr<T>;
        // 构造与析构

        WeakPtr():pPtr(nullptr),Count(nullptr),m_mutex(nullptr){
            std::cout<<"use default no parameter WeakPtr"<<std::endl;
        }

        WeakPtr(SharePtr<T>& sPtr):pPtr(sPtr.pPtr),Count(sPtr.Count),m_mutex(sPtr.m_mutex){
            std::cout<<"use SharePtr to construct WeakPtr"<<std::endl;
            Count->WeakCnt++;
        }
        WeakPtr(WeakPtr<T>& wPtr):pPtr(wPtr.pPtr),Count(wPtr.Count),m_mutex(wPtr.m_mutex){
            std::cout<<" WeakPtr copy construct"<<std::endl;
            Count->WeakCnt++;
        }

        ~WeakPtr(){
            if (Count && Count->WeakCnt > 0) release();
            std::cout<<"finish WeakPtr destruction"<<std::endl;
        }

    public:
        // 符号重载 但是只有 =
        WeakPtr<T>& operator=(const WeakPtr<T>& x){
            if (this != &x){
                release();
                pPtr = x.pPtr;
                m_mutex = x.m_mutex;
                Count = x.Count;
                Count->WeakCnt++;
            }
            return *this;
        }

        WeakPtr<T>& operator=(const SharePtr<T>& x){
            release();
            pPtr = x.pPtr;
            m_mutex = x.m_mutex;
            Count = x.Count;
            Count->WeakCnt++;
            return *this;
        }


    public:
        // 成员函数
        void reset(){
            release();
            pPtr = nullptr, Count = nullptr, m_mutex = nullptr;
        }

        long use_count() const{
            if (!pPtr || !Count) return 0;
            return Count->ShareCnt;
        }

        bool expired() const{
            if (!pPtr) return true;
            if (Count && Count->ShareCnt == 0) return true;
            return false;
        }

        SharePtr<T> lock() const{
            return static_cast<SharePtr<T>>(*this);
        }



    protected:
        void release(){
            if (!Count || Count->WeakCnt <1 ) return; // 保证计数不能小于0 或者 如果这是一个还未依赖于任何共享指针的弱智能指针的时候，都不可以进行减一操作
            std::cout<<"releasing "<<Count->WeakCnt<<" WeakPtr"<<std::endl;
            Count->WeakCnt--;
        }


    private:
        T* pPtr;
        Share_Weak_Count* Count;
        std::mutex* m_mutex;
    };

}



#endif
