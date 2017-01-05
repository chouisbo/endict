#include "endict.h"
#include <datrie/trie.h>
#include <mysql/mysql.h>

#include <iconv.h>
#include <locale.h>

#include <assert.h>
#include <string.h>

/* iconv encoding name for AlphaChar string */
#define ALPHA_ENC   "UCS-4LE"

#define N_ELEMENTS(a)   (sizeof(a)/sizeof((a)[0]))

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

    /* convert to UCS-4LE */
    res = iconv (to_alpha_conv, (char **) &in_p, &in_left,
                 &out_p, &out_left);

    if (res < 0)
        return res;

    /* convert UCS-4LE to AlphaChar string */
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
        sprintf(Query, "SELECT * FROM %s.%s LIMIT 100;", Database, Table);
        printf("Executing %s\n", Query);
        if (! mysql_query(conn, Query)) {
            printf("Executed %s\n", Query);
            MYSQL_RES *res = mysql_use_result(conn);
            if (res) {
                MYSQL_ROW row;
                iconv_t to_alpha_conv = iconv_open (ALPHA_ENC, "UTF-8");
                while ((row = mysql_fetch_row(res))) {
                    gchar *word = strdup(row[0]);
                    gchar *wordtype = strdup(row[1]);
                    gchar *definition = strdup(row[2]);

                        // TODO using glib HashMap, glib string
                        // TODO store word to DATrie
                        const char     *key;
                        AlphaChar       key_alpha[256];
                        TrieData        data;

        key = argv[opt_idx++];
        data = ;

        conv_to_alpha (env, key, key_alpha, N_ELEMENTS (key_alpha));
        if (!trie_store (env->trie, key_alpha, data)) {
            fprintf (stderr, "Failed to add entry '%s' with data %d\n",
                     key, data);
        }
                        printf("%s\t", row[i]);
                    }
                    printf("\n");
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
}

