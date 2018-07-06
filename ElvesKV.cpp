#include <iostream>
#include <cassert>
#include <cstdlib>
#include <string>
#include <leveldb/db.h>

using namespace std;

static bool leveldbGet(leveldb::DB * db, string &key, string &value){
    leveldb::Status s = db->Get(leveldb::ReadOptions(),key,&value);
    assert(s.ok());
    if(s.IsNotFound()){
        return false;
    }else{
        return true;
    }
}
static void leveldbSet(leveldb::DB * db, string key, string value){
    leveldb::Status s = db->Put(leveldb::WriteOptions(),key,value);
    assert(s.ok());
}
int main(){
    leveldb::DB * db;
    leveldb::Options options;
    options.create_if_missing = true;

    // 创建的数据库在 /tmp/testdb
    leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
    assert(status.ok());

    string key = "A";
    string value = "hello world elveskv";
    string get_value;

    leveldbSet(db,key,value);
    leveldbGet(db,key,get_value);
    cout<<get_value<<endl;
    return 0;
}
