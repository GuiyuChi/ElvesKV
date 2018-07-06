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
static bool wisckeyGet(WK * wk, string &key, string &value)
{
    // cout << "\n\t\tGet Function\n\n";
	// cout << "Key Received: " << key << endl;
    // cout << "Value Received: " << value << endl;

    string offsetinfo;
    const bool found = leveldbGet(wk->leveldb, key, offsetinfo);
    if (found) {
        // cout << "Offset and Length: " << offsetinfo << endl;
    }
    else {
        cout << "Record:Not Found" << endl;
        return false;
    }
	string value_offset;
	string value_length;
	string s = offsetinfo;
	string delimiter = "&&";
	size_t pos = 0;
	string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        value_offset = token;
        s.erase(0, pos + delimiter.length());
	}
	value_length = s;

	// cout << "Value Offset: " << value_offset << endl;
	// cout << "Value Length: " << value_length << endl;

  	std::string::size_type sz;
  	long offset = stol (value_offset,&sz);
	long length = stol (value_length,&sz);

	//cout << offset << length << endl;
	char value_record[length];
	//cout << ftell(wk->logread) << endl;
	fseek(wk->logfile,offset,SEEK_SET);
	//cout << ftell(wk->logfile) << endl;
	//rewind(wk->logfile);
	//cout << ftell(wk->logfile) << endl;
	fread(value_record,length,1,wk->logfile);
	//rewind(wk->logfile);
    for(int i=0;i<length;i++){
        cout<<value_record[i];
    }
	return true;
}
// wisckey 写操作封装
static void wisckeySet(WK * wk, string &key, string &value)
{
	long offset = ftell (wk->logfile);
	long size = value.length();
	string vlog_offset = to_string(offset);
	string vlog_size = to_string(size);
	string vlog_value = vlog_offset + "&&" + vlog_size;
    const char* char_value = value.data();
	fwrite (char_value, sizeof(char),size,wk->logfile);
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
// WiscKey 关闭数据库
static void close_wisckey(WK * wk){
    fclose(wk->logfile);
  	delete wk->leveldb;
  	delete wk;
}
// WiscKey 测试程序 写入 读取 删除 确认删除
static void testingWiscKeyFunction(WK * wk, string key, string value){
    string get_value;
    wisckeySet(wk,key,value);
    wisckeyGet(wk,key,get_value);
    // cout<<get_value<<endl;
    // leveldbDel(db,key);
    // assert(leveldbGet(db,key,get_value)==0);
}
// 生成字符串的长度
static size_t randValueSize(){
    // return (size_t)rand()%100+50;
    return 5;
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
    for(int i=0;i<100;i++){
        string key = to_string((size_t)rand());
        string value = randString();
        testingLevelDBFunction(db,key,value);
    }
}
static void wiscKeyTest(){
     WK * wk = open_wisckey("/tmp/wisckey_test_dir");
     if (wk == NULL) {
           cerr << "Open WiscKey failed!" << endl;
           exit(1);
       }
    // string value = "hello world elveskv";
    for(int i=0;i<100;i++){
        string key = to_string((size_t)rand());
        string value = randString();
        testingWiscKeyFunction(wk,key,value);
    }
    string key = "A";
    string value = "hello world elveskv";
    string key1 = "B";
    wisckeySet(wk,key,value);
    cout<<wisckeyGet(wk,key,value)<<endl;
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
    cout << "levelDb time elapsed: " << (clock() - t0) * 1.0e-6 << " seconds" << endl;
    // wisckeySet(wk,key2,value2);
    clock_t t1 = clock();
    wiscKeyTest();
    cout << "wiscKey time elapsed: " << (clock() - t1) * 1.0e-6 << " seconds" << endl;
    return 0;
}
