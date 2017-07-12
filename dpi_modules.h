#ifndef DPI_MODULES_H
#define DPI_MODULES_H

typedef void ( *dpi_module_init_func)(void *userdata)  ;
typedef void ( *dpi_module_helper_func)(char *packet, int len, void *userdata)  ;

typedef struct dpi_module_s{
	char *dpi_name;
	dpi_module_init_func init_func;
	void *init_userdata;
	dpi_module_helper_func helper_func;
	void *helper_userdata;
}dpi_module_t;


#endif
