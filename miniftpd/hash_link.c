	
#include "hash_link.h"

typedef struct hash_node
{
    void *key; //无类型指针，故key可以是任意类型，value同
    void *value; // 有价值数据
    struct hash_node *prev; //前驱指针
    struct hash_node *next; // 后继指针
} hash_node_t;

struct hash
{
    unsigned int buckets; //桶的个数
    hashfunc_t hash_func; // 哈希函数
    hash_node_t **nodes; //指向哈希表数组的指针，数组放的是hash_node_t*
};


hash_node_t **hash_get_bucket(hash_t *hash, void *key);
hash_node_t *hash_get_node_by_key(hash_t *hash, void *key, unsigned int key_size);

hash_t *hash_alloc(unsigned int buckets, hashfunc_t hash_func)
{
    hash_t *hash = malloc(sizeof(hash_t));
    hash->buckets = buckets;
    hash->hash_func = hash_func;
    int size = buckets * sizeof(hash_node_t *); // 哈希表数组的大小
    hash->nodes = (hash_node_t **)malloc(size);
    memset(hash->nodes, 0, size); // 数组清0

    printf("The hash table has allocate.\n");

    return hash;

}

void hash_free(hash_t *hash)
{
    unsigned int buckets = hash->buckets;
    int i;
    for (i = 0; i < buckets; i++)
    {
        while (hash->nodes[i] != NULL)
        {
            hash_node_t *tmp = hash->nodes[i];
            hash->nodes[i] = tmp->next;
            if (tmp->next != NULL)  //也许只有一个节点
                tmp->next->prev = tmp->prev;
            free(tmp->value);
            free(tmp->key);
            free(tmp);
        }
    }

    free(hash);
    printf("The hash table has free.\n");

}

void *hash_lookup_entry(hash_t *hash, void *key, unsigned int key_size)
{
    hash_node_t *node = hash_get_node_by_key(hash, key, key_size);
    if (node == NULL)
        return NULL;
    return node->value;
}


void hash_add_entry(hash_t *hash, void *key, unsigned int key_size, void *value, unsigned int value_size)
{
    if (hash_lookup_entry(hash, key, key_size))
    {
        // key 值已经存在，直接返回
        fprintf(stderr, "duplicate hash key\n");
        return ;
    }

    hash_node_t *node = malloc(sizeof(hash_node_t));
    node->prev = NULL;
    node->next = NULL;
    node->key = malloc(key_size);
    memcpy(node->key, key, key_size);
    node->value = malloc(value_size);
    memcpy(node->value, value, value_size);

    // 插入第一个结点
    hash_node_t **bucket = hash_get_bucket(hash, key);
    if (*bucket == NULL)
        *bucket = node;

    else   //将新结点插入链表头部
    {
        node->next = *bucket;
        (*bucket)->prev = node;
        *bucket = node;
    }
}


void hash_free_entry(hash_t *hash, void *key, unsigned int key_size)
{

    hash_node_t *node = hash_get_node_by_key(hash, key, key_size);
    if (node == NULL)
        return;

    free(node->key);
    free(node->value);

    // 双向链表的删除操作
    if (node->prev != NULL)
        node->prev->next = node->next;

    else
    {
        hash_node_t **bucket = hash_get_bucket(hash, key);
        *bucket = node->next;
    }

    if (node->next != NULL)
        node->next->prev = node->prev;

    free(node);
}


hash_node_t **hash_get_bucket(hash_t *hash, void *key)
{
    // 通过哈希函数返回地址
    unsigned int bucket = hash->hash_func(hash->buckets, key);
    if (bucket >= hash->buckets)
    {
        fprintf(stderr, "bad bucket lookup\n");
        exit(EXIT_FAILURE);
    }

    return &(hash->nodes[bucket]); //返回指向某个桶的指针

}


hash_node_t *hash_get_node_by_key(hash_t *hash, void *key, unsigned int key_size)
{
    hash_node_t **bucket = hash_get_bucket(hash, key);
    hash_node_t *node = *bucket;
    if (node == NULL)
    {
        return NULL;
    }

    while (node != NULL && memcmp(node->key, key, key_size) != 0)
    {
        // 通过链表头指针不断查询是否匹配
        node = node->next;
    }

    return node;
}
