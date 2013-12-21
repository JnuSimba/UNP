	
#include "hash_link.h"

typedef struct hash_node
{
    void *key; //������ָ�룬��key�������������ͣ�valueͬ
    void *value; // �м�ֵ����
    struct hash_node *prev; //ǰ��ָ��
    struct hash_node *next; // ���ָ��
} hash_node_t;

struct hash
{
    unsigned int buckets; //Ͱ�ĸ���
    hashfunc_t hash_func; // ��ϣ����
    hash_node_t **nodes; //ָ���ϣ�������ָ�룬����ŵ���hash_node_t*
};


hash_node_t **hash_get_bucket(hash_t *hash, void *key);
hash_node_t *hash_get_node_by_key(hash_t *hash, void *key, unsigned int key_size);

hash_t *hash_alloc(unsigned int buckets, hashfunc_t hash_func)
{
    hash_t *hash = malloc(sizeof(hash_t));
    hash->buckets = buckets;
    hash->hash_func = hash_func;
    int size = buckets * sizeof(hash_node_t *); // ��ϣ������Ĵ�С
    hash->nodes = (hash_node_t **)malloc(size);
    memset(hash->nodes, 0, size); // ������0

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
            if (tmp->next != NULL)  //Ҳ��ֻ��һ���ڵ�
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
        // key ֵ�Ѿ����ڣ�ֱ�ӷ���
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

    // �����һ�����
    hash_node_t **bucket = hash_get_bucket(hash, key);
    if (*bucket == NULL)
        *bucket = node;

    else   //���½���������ͷ��
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

    // ˫�������ɾ������
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
    // ͨ����ϣ�������ص�ַ
    unsigned int bucket = hash->hash_func(hash->buckets, key);
    if (bucket >= hash->buckets)
    {
        fprintf(stderr, "bad bucket lookup\n");
        exit(EXIT_FAILURE);
    }

    return &(hash->nodes[bucket]); //����ָ��ĳ��Ͱ��ָ��

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
        // ͨ������ͷָ�벻�ϲ�ѯ�Ƿ�ƥ��
        node = node->next;
    }

    return node;
}
