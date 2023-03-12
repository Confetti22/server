//嵌套类+静态对象
#ifndef _AUTO1_
#define _AUTO1_

#include<iostream>
using std::cout;
using std::endl;
/* class SingleTon; */ 
//自动释放

//递归调用时的出口,必须要在上面出现
#ifdef DISPLAY

//display函数的定义
void display(){}

    template <class T, class... Args>
void display(T t, Args... args)
{
    cout << t << ",";
    display(args...);//递归调用
}

#endif

template<class T>
class SingleTon{//AutoRelease类型只为Singleton服务
    //因此设计成private,不需要在外部创建
private:
    class AutoRelease{
    public:
        AutoRelease(){	cout << "AutoRelease(p)" << endl;}

        ~AutoRelease()
        {
            cout << "~AutoRelease()" << endl;
            if(_pInstance) {
                delete _pInstance;
            }
        }
    };
public:
    //不希望对象被复制构造， 则需要删除复制控制语言函数
    SingleTon(const SingleTon&)=delete;
    SingleTon& operator=(const SingleTon&)=delete;

    //getInstance 只在第一次调用时创建一个该类型的堆对象，
    //再次调用时，直接返回指向该对象的指针
    //③定义一个返回值为类指针的静态成员函数
    template<class ...Args>
        static T * getInstance(Args&& ...args) { 
#ifdef DISPLAY
            cout<<"getInstance(";
            display(args...);
            cout<<")" << endl;
#endif
            if (nullptr == _pInstance){
                //以下两种形式都可
                // _pInstance = new T(std::forward<Args>(args)...);
                _pInstance = new T(args...);
                _ar;

            }
            return _pInstance;
        }

    static void destroy(){
        if(_pInstance){
            delete _pInstance;
            _pInstance=nullptr;
        }
    }

    void show()const{
        _pInstance->show();
    }


private:
    //①将构造函数私有化   
    SingleTon(){
        cout<<"calling constrcutor SingleTon()"<<endl;
    }
    ~SingleTon(){
        cout<<"callint deconstructor ~SingleTon()"<<endl;
    }

private:
    static T* _pInstance;//②在类中定义一个静态的指向本类型的指针变量
    static AutoRelease _ar; //静态的对象成员 _ar

};


template <class T> 
T* SingleTon<T>::_pInstance = nullptr;

//静态对象成员的定义
template <class T> 
typename SingleTon<T>::AutoRelease SingleTon<T>::_ar;

#endif
