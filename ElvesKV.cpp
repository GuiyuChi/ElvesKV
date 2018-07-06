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
    // cout<<get_value<<endl;
    leveldbDel(db,key);
    assert(leveldbGet(db,key,get_value)==0);
}
// 生成字符串的长度
static size_t randValueSize(){
    return (size_t)rand()/100000 + 5000;
}
// 生成随机字符串
static string randString(){
    const size_t value_size = randValueSize();
    // cout<<value_size<<endl;
    char * vbuf = new char[value_size];
    for(size_t i=0;i<value_size;i++){
        vbuf[i]=rand();
    }
    string value = string(vbuf,value_size);
    return value;
}
static void levelDbTest(){
    leveldb::DB * db = open_leveldb("/tmp/testdb");
    // string value = "hello world elveskv";
    for(int i=0;i<1000;i++){
        string key = to_string((size_t)rand());
        string value = randString();
        testingLevelDBFunction(db,key,value);
    }
}
int main(){
    // leveldb::DB * db = open_leveldb("/tmp/testdb");
    // string key = "A";
    // string value = "hello world elveskv";
    // string get_value;
    // testingLevelDBFunction(db,key,value);
    // cout << "time elapsed: " << levelDbTest() * 1.0e-6 << " seconds" << endl;
    clock_t t0 = clock();
    levelDbTest();
    clock_t dt = clock() - t0;
    cout << "time elapsed: " << dt * 1.0e-6 << " seconds" << endl;
    return 0;
}
