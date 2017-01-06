#include "endict.h"
#include <datrie/trie.h>
#include <mysql/mysql.h>

#include <assert.h>
#include <string.h>
#include <ctype.h>

#include <iconv.h>
// iconv encoding name for AlphaChar string
#define ALPHA_ENC   "UCS-4LE"

#define N_ELEMENTS(a)   (sizeof(a)/sizeof((a)[0]))

/* struct AssociatedDataNode { */
/*     const char *word; */
/*     const char *wordtype; */
/*     const char *definition; */

/*     AssociatedDataNode *next; */
/* }; */

/* #define ASSOCIATED_DATA_LENGTH 200000 */
/* static AssociatedDataNode *as_data[ASSOCIATED_DATA_LENGTH]; */

static size_t
conv_to_alpha (iconv_t to_alpha_conv, const char *in, AlphaChar *out, size_t out_size)
{
    char   *in_p = (char *) in;
    char   *out_p = (char *) out;
    size_t  in_left = strlen (in);
    size_t  out_left = out_size * sizeof (AlphaChar);
    size_t  res;
    const unsigned char *byte_p;

    assert (sizeof (AlphaChar) == 4);

    // convert to UCS-4LE
    res = iconv (to_alpha_conv, (char **) &in_p, &in_left,
                 &out_p, &out_left);

    if (res < 0)
        return res;

    // convert UCS-4LE to AlphaChar string
    res = 0;
    for (byte_p = (const unsigned char *) out;
         res < out_size && byte_p + 3 < (unsigned char*) out_p;
         byte_p += 4)
    {
        out[res++] = byte_p[0]
                     | (byte_p[1] << 8)
                     | (byte_p[2] << 16)
                     | (byte_p[3] << 24);
    }
    if (res < out_size) {
        out[res] = 0;
    }

    return res;
}

void to_lower_cstring(char *str) {
    char *p = str;
    while (*p != '\0') {
        *p = (char) tolower(*p);
        p++;
    }
}

ENDict endict_init_from_db(const char* jdbc) {

    char Host[256] = {'\0'};
    unsigned int Port = 0;
    char User[256] = {'\0'};
    char Password[256] = {'\0'};
    char Database[256] = {'\0'};
    char Table[256] = {'\0'};

    int rc = sscanf(jdbc, "-h %s -P %d -u %s -p %s %s %s", Host, &Port, User, Password, Database, Table);
    /* printf("host => %s\n", Host); */
    /* printf("port => %d\n", Port); */
    /* printf("user => %s\n", User); */
    /* printf("password => %s\n", Password); */
    /* printf("database => %s\n", Database); */
    /* printf("table => %s\n", Table); */
    assert(6 == rc);

    AlphaMap *am = alpha_map_new();
    alpha_map_add_range(am, 0, 255);
    Trie *trie = trie_new(am);
    alpha_map_free(am);

    MYSQL *conn = mysql_init(NULL);
    if (! mysql_real_connect(conn, Host, User, Password, Database, Port, NULL, 0)) {
        fprintf(stderr, "Error connecting to database: %s\n", mysql_error(conn));
    } else { // Connected to MYSQL
        char Query[512] = {'\0'};
        sprintf(Query, "SELECT * FROM %s.%s ORDER BY word;", Database, Table);
        printf("Executing %s\n", Query);
        if (! mysql_query(conn, Query)) {
            printf("Executed %s\n", Query);
            MYSQL_RES *res = mysql_use_result(conn);
            if (res) {
                MYSQL_ROW row;
                iconv_t to_alpha_conv = iconv_open(ALPHA_ENC, "UTF-8");
                TrieData dataIndex = 0;
                while ((row = mysql_fetch_row(res))) {
                    const char *key = row[0];
                    char *k = strdup(key);
                    to_lower_cstring(k);
                    AlphaChar key_alpha[256];

                    conv_to_alpha(to_alpha_conv, k, key_alpha, N_ELEMENTS(key_alpha));

                    if (! trie_store_if_absent(trie, key_alpha, ++dataIndex)) {
                        fprintf(stderr, "Failed to add entry '%s' with dataIndex %d\n", key, dataIndex);
                        TrieData currentIndex = -1;
                        if (trie_retrieve (trie, key_alpha, &currentIndex)) {
                        //TODO
                        
                        }
                    } else {
                        fprintf(stdout, "add entry '%s' with dataIndex %d\n", key, dataIndex);
                        // TODO
                        
                    }
                    if (k != NULL) {
                        free(k);
                    }
                }
                iconv_close(to_alpha_conv);
            }
            mysql_free_result(res);
        }
    }
    mysql_close(conn);

    return (void *) trie;
}

ENDict endict_init_from_dict(const char* dict_filepath) {
    Trie *trie = trie_new_from_file(dict_filepath);
    return (void *) trie;
}

void endict_save_to_dict(ENDict endict, const char* dict_filepath) {
    trie_save((Trie *) endict, dict_filepath);
}

void endict_free(ENDict endict) {
    if (endict != NULL) {
        trie_free((Trie *) endict);
        endict = NULL;
    }

    /* for (int i = 0; i < ASSOCIATED_DATA_LENGTH; i++) { */
    /*     if (as_data[i] != NULL) { */
    /*     } */
    /* } */
}

const char *endict_query(ENDict endict, const char *query) {
    AlphaChar key_alpha[256];

    char *q = strdup(query);
    to_lower_cstring(q);
    iconv_t to_alpha_conv = iconv_open(ALPHA_ENC, "UTF-8");
    conv_to_alpha(to_alpha_conv, q, key_alpha, N_ELEMENTS(key_alpha));
    iconv_close(to_alpha_conv);

    TrieData dataIndex = -1;
    if (trie_retrieve ((Trie *) endict, key_alpha, &dataIndex)) {
        fprintf(stdout, "retrieve %s with dataIndex %d\n", query, dataIndex);
    } else {
        fprintf(stderr, "can not retrieve %s\n", query);
    }
    if (q != NULL) {
        free(q);
    }

    return NULL;
}

