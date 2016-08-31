/*
 * Copyright (c) 2009-2014 Petri Lehtinen <petri@digip.org>
 * Copyright (c) 2011-2012 Basile Starynkevitch <basile@starynkevitch.net>
 *
 * Jansson is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <stdlib.h>
#include <string.h>

#include "jansson.h"
#include "jansson_private.h"

#ifdef JANSSON_USE_CHIBIOS
  #include "ch.h"
#endif
#ifdef JANSSON_USE_BLF
  extern void BLF_free(void *);
  extern void *BLF_alloc(unsigned int size);

#endif

/* C89 allows these to be macros */
#undef malloc
#undef free

/* memory function pointers */
//static json_malloc_t do_malloc = malloc;
//static json_free_t do_free = free;

void *jsonp_malloc(size_t size)
{
    if(!size)
        return NULL;
#ifdef JANSSON_USE_CHIBIOS
    return chHeapAlloc(0, size);
#endif
#ifdef JANSSON_USE_BLF
    return BLF_alloc(size);
#endif
    //return (*do_malloc)(size);
}

void jsonp_free(void *ptr)
{
    if(!ptr)
        return;

    //(*do_free)(ptr);
#ifdef JANSSON_USE_CHIBIOS
    chHeapFree(ptr);
#endif
#ifdef JANSSON_USE_BLF
    BLF_free(ptr);
#endif
}

char *jsonp_strdup(const char *str)
{
    return jsonp_strndup(str, strlen(str));
}

char *jsonp_strndup(const char *str, size_t len)
{
    char *new_str;

    new_str = jsonp_malloc(len + 1);
    if(!new_str)
        return NULL;

    memcpy(new_str, str, len);
    new_str[len] = '\0';
    return new_str;
}

void json_set_alloc_funcs(json_malloc_t malloc_fn, json_free_t free_fn)
{
    //do_malloc = malloc_fn;
   // do_free = free_fn;
}
