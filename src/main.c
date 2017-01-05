#include <stdio.h>
#include <string.h>

#include <glib.h>
#include "endict.h"

static gchar *mode = NULL;
static gchar *db = NULL;
static gchar *dict = NULL;

static GOptionEntry entries[] = {
    { "mode", NULL, 0, G_OPTION_ARG_STRING, &mode, "save: save dictionary to file; cli: query command line", "save, cli" },
    { "db", NULL, 0, G_OPTION_ARG_STRING, &db, "mysql db formatted info", "-h 10.61.1.29 -P 3306 -u root -p root entries entries" },
    { "dict", NULL, 0, G_OPTION_ARG_STRING, &dict, "dictionary file path; mode:save => output, mode:cli => input", "./dictionary.dat" },
    { NULL }
};

int main(const int argc, const char *argv[]) {

    GError *error = NULL;
    const char *UsageInfo = " - English Dictionary Command Line Tool";
    GOptionContext *context = g_option_context_new(UsageInfo);
    g_option_context_add_main_entries(context, entries, NULL);
    if (! g_option_context_parse(context, &argc, (gchar ***) &argv, &error)) {
        g_print("option parsing failed: %s\n", error->message);
        return 1;
    }

    if (NULL == mode) {
        fprintf(stderr, "mode must be set.\n");
        return 1;
    }
    if (! strcmp(mode, "save")) {
        if (NULL == db || NULL == dict) {
            fprintf(stderr, "db and dict should be set.\n");
            return 1;
        }
        ENDict endict = endict_init_from_db(db);
        /* endict_save_to_dict(endict, dict); */
        endict_free(endict);
    } else if (! strcmp(mode, "cli")) {
        ENDict endict = NULL; 
        if (dict != NULL) {
            endict = endict_init_from_dict(dict);
        } else if (db != NULL) {
            endict = endict_init_from_db(db);
        } else {
            fprintf(stderr, "db or dict should be set.\n");
            return 1;
        }
        endict_free(endict);
    } else {
        fprintf(stderr, "unknown mode = %s.\n", mode);
        return 1;
    }

    g_option_context_free(context);
    return 0;
}

