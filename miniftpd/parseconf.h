#ifndef _PARSE_CONF_H_
#define _PARSE_CONF_H_

void parseconf_load_file(const char *path);
void parseconf_load_setting(const char *setting);

typedef struct parseconf_bool_setting
{
  const char *p_setting_name;
  int *p_variable;
} BOOL_CONFIG;



typedef struct parseconf_uint_setting
{
	const char *p_setting_name;
	unsigned int *p_variable;
} UINT_CONFIG;


typedef struct parseconf_str_setting
{
	const char *p_setting_name;
	const char **p_variable;
} STR_CONFIG;


#endif //parseconf.h