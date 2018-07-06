#include <iostream>
#include <cassert>
#include <cstdlib>
#include <string>
#include <leveldb/db.h>

using namespace std;

// leveldb 读操作封装
static bool leveldbGet(leveldb::DB * db, string &key, string &value){
    leveldb::Status s = db->Get(leveldb::ReadOptions(),key,&value);
    assert(s.ok());
    if(s.IsNotFound()){
        return false;
    }else{
        return true;
    }
}

// leveldb 写操作封装
static void leveldbSet(leveldb::DB * db, string key, string value){
    leveldb::Status s = db->Put(leveldb::WriteOptions(),key,value);
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

int main(){
    leveldb::DB * db = open_leveldb("/tmp/testdb");
    string key = "A";
    string value = "hello world elveskv 1";
    string get_value;

    leveldbSet(db,key,value);
    leveldbGet(db,key,get_value);
    cout<<get_value<<endl;
    return 0;
}
