#ifndef _HASH_LINK_H_
#define _HASH_LINK_H_
#include "common.h"

/* 给定关键码key，经过哈希函数计算得到的是关键码对应的数据项在数组中的存储下标index/bucket
 * 数据项所存储的表用数组实现，即hash table
 */

typedef struct hash hash_t;
typedef unsigned int (*hashfunc_t)(unsigned int, void *); // 第一个参数是桶的个数(地址范围），第二个参数是key值指针

hash_t *hash_alloc(unsigned int buckets, hashfunc_t hash_func); // 建立哈希表
void hash_free(hash_t *hash); // 释放哈希表
void *hash_lookup_entry(hash_t *hash, void *key, unsigned int key_size); //在哈希表中查找一项key
// 在哈希表中添加一条记录
void hash_add_entry(hash_t *hash, void *key, unsigned int key_size, void *value, unsigned int value_size);
void hash_free_entry(hash_t *hash, void *key, unsigned int key_size); // 在哈希表中删除一条记录


#endif