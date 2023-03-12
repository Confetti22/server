//atexit+destory
#include<iostream>
using std::cout;
using std::endl;
/* class SingleTon; */ 
//自动释放
//
//递归调用时的出口,必须要在上面出现
#if 0
void display()
{}

    template <class T, class... Args>
void display(T t, Args... args)
{
    cout << t << ",";
    display(args...);//递归调用
}
#endif


template<class T>
class SingleTon{
public:
    SingleTon(const SingleTon&)=delete;
    SingleTon& operator=(const SingleTon&)=delete;

    template<class ...Args>
        static T * getInstance(Args&& ...args) { 
            cout<<"getInstance(";
            /* display(args...); */
            cout<<")" << endl;
            if (nullptr == _pInstance){
                atexit(destroy);
                _pInstance = new T(args...);

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
};


//饿汉模式，在进入main()函数之前就先创建了实例
template <class T> 
T* SingleTon<T>::_pInstance = getInstance();//饿汉模式


