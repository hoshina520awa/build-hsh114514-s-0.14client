#pragma once

#include "StrBuild.h"
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <link.h>
#include <dlfcn.h>


int Findvtableindex(const char *vtable_sym,const char *func_sym ,int size);        
void* GetVtableFunc(const char *vtable_sym, int index);
void Vtablehook(const char *vtable_sym,int index, void *replace, void **result);
//bool Vtablehook(const char *vtable_sym,const char *func_sym, void *replace, void **result);
