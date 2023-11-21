#ifndef __MOXU_SINGLETON_H__
#define __MOXU_SINGLETON_H__

#include<memory>

namespace moxu{

//单例模式的模板类
template<typename T,typename X = void,int N = 0>  //N是用来区分；类的示例
class Singleton{
    public:
        static T* GetInstance(){
            static T v;
            return &v;
        }

};
template<class T,class X = void,int N = 0>
class SingletonPtr{
    public:
        static std::shared_ptr<T> GetInstance(){
            static std::shared_ptr<T> v(new T);
            return v;
        }
};

}



#endif