#ifndef __MOXU_CONFIG_H__
#define __MOXU_CONFIG_H__

#include<memory>
#include<sstream>
#include<string>
#include<boost/lexical_cast.hpp>
#include<yaml-cpp/yaml.h>
#include<typeinfo>
#include"log.h"


/*lookup层转换为小写 -- 记*/

namespace moxu{

//配置项基类
class ConfigVarBase{  //抽象配置项
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(const std::string & name,const std::string & description = ""):
            m_name(name),
            m_description(description){ 
                std::transform(m_name.begin(),m_name.end(),m_name.begin(),
                [](char& c){
                    return std::tolower(c);
                }); //直接转换为小写
            }

    virtual ~ConfigVarBase() {}   //方便释放内存

    const std::string get_Name() const { return m_name; }
    const std::string get_Description() const { return m_description; }

    virtual std::string toString() = 0;
    virtual bool fromString(const std::string& val) = 0;

protected:
    std::string m_name;
    std::string m_description;
};

//基础类型的转换类 使用boost::lexical_cast<dis>(src)
//F from_type T to_type
template<class F,class T>
class LexicalCast{
public:
    T operator()(const F& v){
            return boost::lexical_cast<T>(v);
    }
};


//偏特化 --vec
template<class T>
class LexicalCast<std::string,std::vector<T> >{
public:
    std::vector<T> operator()(const std::string& v){
            YAML::Node node = YAML::Load(v);  //string->结构
            typename std::vector<T> vec;
            using vec_ = std::vector<T>;

            std::stringstream ss;

            for(size_t i = 0;i < node.size() ; ++i){
                vec.push_back(typename LexicalCast<std::string,T>()(ss.str()));
            }
            return vec;
    }
};



//配置项类

//FromString T operator()(const std::string);  仿函数  通过string转成我们的类型
//ToStr std::string operator()(const T&);
template<class T,class FromStr=LexicalCast<std::string,T>
                 ,class ToStr=LexicalCast<T,std::string> > //特例化
class ConfigVar : public ConfigVarBase{  //一个具体的配置项
public:
    typedef  std::shared_ptr<ConfigVar> ptr; //shared_ptr<ConfigVar>的类型别名
    ConfigVar(const std::string& name
            ,const T& default_value
            ,const std::string& description="")
            :ConfigVarBase(name,description),m_val(default_value){  }

    std::string toString() override{ //告诉编译器这个方法是父类继承来的抽象函数，如果不是就会报错
        try{
                //boost::lexical_cast<std::string>(m_val);  //lexical_case 将成员类型转换为string类型
                return ToStr()(m_val);
        }catch(std::exception e){
            MOXU_LOG_ERROR(MOXU_LOG_ROOT())<<"ConfigVar::toString execption"<<e.what()
                    <<" convert: "<<typeid(m_val).name()<<" to string"; //typeid返回表达式的类型信息
        }
        return "";
    }

    bool fromString(const std::string& val) override{
        try{
                //m_val=boost::lexical_cast<T>(val); // 将string类型转换为成员类型
                setValue(FromStr()(val));
        }catch(std::exception e){
            MOXU_LOG_ERROR(MOXU_LOG_ROOT())<<"ConfigVar::fromString execption"<<e.what()
                    <<" convert: string to"; //typeid返回表达式的类型信息
        }
        return false;
    }


    const T getValue() const {
        return m_val;
    }

    void setValue(const T&val){
        m_val=val;
    }

private:
    T m_val;
};

//配置管理类  -- 配置项的注册，查找，加载......
class Config{
public:
    typedef std::map<std::string,ConfigVarBase::ptr> ConfigVarMap;   
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string & name,
        const T&default_value,const std::string description=""){
            auto tmp=Lookup<T>(name);
            if(tmp){
                MOXU_LOG_INFO(MOXU_LOG_ROOT()) << "Lookup name="<<name<<"exists";
                return tmp;
            }
            if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._0123456789")
                !=std::string::npos)
                {
                    MOXU_LOG_ERROR(MOXU_LOG_ROOT()) << "Lookup name invalid "<<name;
                    throw std::invalid_argument(name); //输入的参数值无效
                }

                typename ConfigVar<T>::ptr v(new ConfigVar<T>(name,default_value,description));
                s_datas[name]=v;
                std::cout<<"插入"<<std::endl;
                return v;
        }

    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string name){ //查找
        auto it = s_datas.find(name);
        if(it->second){
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T>>(it->second); //dynamic_pointer_cast()将/一个派生类智能ptr转换为基类ptr
    }

    static void LoadFromYaml(const YAML::Node& root);

    static ConfigVarBase::ptr LookupBase(const std::string& name); //查找有没有当前的项 如果有的话返回
private:
    static ConfigVarMap s_datas;
};

}

#endif