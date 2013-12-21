#ifndef _HASH_LINK_H_
#define _HASH_LINK_H_
#include "common.h"

/* �����ؼ���key��������ϣ��������õ����ǹؼ����Ӧ���������������еĴ洢�±�index/bucket
 * ���������洢�ı�������ʵ�֣���hash table
 */

typedef struct hash hash_t;
typedef unsigned int (*hashfunc_t)(unsigned int, void *); // ��һ��������Ͱ�ĸ���(��ַ��Χ�����ڶ���������keyֵָ��

hash_t *hash_alloc(unsigned int buckets, hashfunc_t hash_func); // ������ϣ��
void hash_free(hash_t *hash); // �ͷŹ�ϣ��
void *hash_lookup_entry(hash_t *hash, void *key, unsigned int key_size); //�ڹ�ϣ���в���һ��key
// �ڹ�ϣ�������һ����¼
void hash_add_entry(hash_t *hash, void *key, unsigned int key_size, void *value, unsigned int value_size);
void hash_free_entry(hash_t *hash, void *key, unsigned int key_size); // �ڹ�ϣ����ɾ��һ����¼


#endif