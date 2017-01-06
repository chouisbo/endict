#ifndef __ENDICT_H__
#define __ENDICT_H__

typedef void* ENDict;

ENDict endict_init_from_db(const char* jdbc);
ENDict endict_init_from_dict(const char* dict_filepath);
void endict_save_to_dict(ENDict endict, const char* dict_filepath);
const char *endict_query(ENDict endict, const char *query);

void endict_free(ENDict endict);


#endif // __ENDICT_H__

