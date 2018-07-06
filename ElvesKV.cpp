#include <iostream>
#include <cassert>
#include <cstdlib>
#include <string>
#include <leveldb/db.h>

using namespace std;

typedef struct WiscKey{
    string dir;
    leveldb::DB * leveldb;
    FILE * logfile;
}WK;
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
// wisckey 读操作封装
static bool wisckeyGet(WK * wk, string &key, string &value){
    string offsetinfo;
    const bool found = leveldbGet(wk->leveldb,key,offsetinfo);
    // cout<<offsetinfo<<endl;
    if(!found){
        cout << "Record:Not Found" << endl;
    }
    string value_offset;
    string value_length;
    string s = offsetinfo;
    string delimiter = "&&";
    size_t pos = 0;
    string token;
    while((pos = s.find(delimiter)) != string::npos){
        token = s.substr(0,pos);
        value_offset = token;
        s.erase(0, pos + delimiter.length());
    }
    value_length = s;
    long offset = stol(value_offset);
    long length = stol(value_length);
    // cout<<offset<<endl;
    // cout<<length<<endl;
    fseek(wk->logfile,offset,SEEK_SET);
    fread(&value,length,1,wk->logfile);
    return true;
}
// wisckey 写操作封装
static void wisckeySet(WK* wk, string &key, string &value){
    long offset = ftell(wk->logfile);
    //修改了源码
    long size = value.length();
    string vlog_offset = to_string(offset);
    string vlog_size = to_string(size);
    // cout<<vlog_offset<<endl;
    // cout<<vlog_size<<endl;
    string vlog_value = vlog_offset+"&&"+vlog_size;
    fwrite(&value,sizeof(value),1,wk->logfile);
    leveldbSet(wk->leveldb,key,vlog_value);
}
// WiscKey 创建数据库
static WK * open_wisckey(const string& dirname){
    WK * wk = new WK;
    wk->leveldb = open_leveldb(dirname);
    wk->dir = dirname;
    wk->logfile = fopen("logfile","wb+");
    return wk;
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
    // clock_t t0 = clock();
    // levelDbTest();
    // clock_t dt = clock() - t0;
    // cout << "time elapsed: " << dt * 1.0e-6 << " seconds" << endl;
    WK * wk = open_wisckey("/tmp/wisckey_test_dir");
    if (wk == NULL) {
    		cerr << "Open WiscKey failed!" << endl;
    		exit(1);
  	}
    string key1 = "adf";
    string value1 = "hello wisc";
    string key2 = "abcd";
    string value2 = "wodsfdsfadfasfrfrafgfdfawereffeawfdsfef";
    string key3 = "adfafsf";
    string value3 = "dfsasdfgasadgewrgfdvrgfbgd";
    wisckeySet(wk,key1,value1);
    wisckeySet(wk,key2,value2);
    wisckeySet(wk,key3,value3);
    wisckeyGet(wk,key3,value1);
    cout<<value1<<endl;
    return 0;
}
