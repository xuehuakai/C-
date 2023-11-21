#include"../src/config.h"
#include"../src/log.h"
#include<yaml-cpp/yaml.h>

moxu::ConfigVar<int>::ptr g_int_value_config = 
        moxu::Config::Lookup("system.port",(int)8080,"system port");
moxu::ConfigVar<float>::ptr g_float_value_config=
        moxu::Config::Lookup("system.value",(float)10.2f,"system value");

moxu::ConfigVar<std::vector<int> >::ptr g_int_vec_value_config=
        moxu::Config::Lookup("system.int_vec",std::vector<int>{1,2},"system int vec");
// moxu::ConfigVar<std::list<int> >::ptr g_int_list_value_config=
//         moxu::Config::Lookup("system.int_list",std::list<int>{1,2},"system int list");

moxu::ConfigVar<std::list<int> >::ptr g_int_list_value_config=
        moxu::Config::Lookup("system.int.list",std::list<int>{2,3},"system int list");

void print_yaml(const YAML::Node &node,int level){
    if(node.IsScalar()){ //如果是标量节点：字符串，数字，布尔值.
        MOXU_LOG_INFO(MOXU_LOG_ROOT()) <<std::string(level*4,' ')
            <<node.Scalar()<<" - "<<node.Type()<<" - "<<level<<" Scalar";
    }else if(node.IsNull()){
        MOXU_LOG_INFO(MOXU_LOG_ROOT()) <<std::string(level*4,' ')
            <<"NULL - "<<" - "<<node.Type()<<level;
    }else if (node.IsMap()){
        for(auto it = node.begin();
                it!=node.end();it++){
            MOXU_LOG_INFO(MOXU_LOG_ROOT()) <<std::string(level*4,' ')
                <<it->first<<" - "<<it->second.Type()<<" - "<<level<<" Map";
            print_yaml(it->second,level+1);
        }
    }else if(node.IsSequence()){
        for(size_t i = 0;i<node.size();i++){
            MOXU_LOG_INFO(MOXU_LOG_ROOT())<<std::string(level*4,' ')
                <<i<<" - "<<node[i].Type()<<" - "<<level<<" "<<"Sequence";
            print_yaml(node[i],level+1);
        }
        }
}

void test_ymal(){
    YAML::Node root = YAML::LoadFile("/a/workspace/moxu/bin/conf/log.yml");

    print_yaml(root,0);

    MOXU_LOG_INFO(MOXU_LOG_ROOT()) <<root.Scalar();  //将文件信息输出到控制台中
}

void test_config(){
    MOXU_LOG_INFO(MOXU_LOG_ROOT())<<"before: "<<g_int_value_config->getValue();
    MOXU_LOG_INFO(MOXU_LOG_ROOT())<<"before: "<<g_float_value_config->toString();

    auto&v = g_int_vec_value_config->getValue();//先把值拿出来
    for(auto&i:v){
        MOXU_LOG_INFO(MOXU_LOG_ROOT())<<"int_vec: "<<i;
    }


    YAML::Node root = YAML::LoadFile("/a/workspace/moxu/bin/conf/log.yml");
    moxu::Config::LoadFromYaml(root);

    MOXU_LOG_INFO(MOXU_LOG_ROOT())<<"after: "<<g_int_value_config->getValue();
    MOXU_LOG_INFO(MOXU_LOG_ROOT())<<"after: "<<g_float_value_config->toString();
    
}


int main(int argc,char** argv){
    test_config();
    return 0;
}
