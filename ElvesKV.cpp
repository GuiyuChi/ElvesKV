#include <iostream>
#include <cassert>
#include <cstdlib>
#include <string>
#include <leveldb/db.h>

using namespace std;

// leveldb 读操作封装
static bool leveldbGet(leveldb::DB * db, string &key, string &value){
    leveldb::Status s = db->Get(leveldb::ReadOptions(),key,&value);
    // assert(s.ok());
    if(s.IsNotFound()){
        return false;
    }else{
        return true;
    }
}

// leveldb 写操作封装 暂时采用PUT方法
static void leveldbSet(leveldb::DB * db, string key, string value){
    leveldb::Status s = db->Put(leveldb::WriteOptions(),key,value);
    assert(s.ok());
}

// level 删除操作方法封装
static void leveldbDel(leveldb::DB * db,string &key){
    leveldb::Status s = db->Delete(leveldb::WriteOptions(),key);
    assert(s.ok());
}

// leveldb 建库操作封装
static leveldb::DB * open_leveldb(const string &dirname){
    leveldb::DB * db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status s = leveldb::DB::Open(options,dirname,&db);
    assert(s.ok());
    return db;
}
// leveldb 测试程序 写入 读取 删除 确认删除
static void testingLevelDBFunction(leveldb::DB * db, string key, string value){
    string get_value;
    leveldbSet(db,key,value);
    leveldbGet(db,key,get_value);
    cout<<get_value<<endl;
    leveldbDel(db,key);
    assert(leveldbGet(db,key,get_value)==0);
}
int main(){
    leveldb::DB * db = open_leveldb("/tmp/testdb");
    string key = "A";
    string value = "hello world elveskv";
    string get_value;
    testingLevelDBFunction(db,key,value);
    return 0;
}
