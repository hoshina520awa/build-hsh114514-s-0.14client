#include "VtableHook.h"
#include "main.h"

int Findvtableindex(const char *vtable_sym, const char *func_sym, int max_funcs) {
    void *lib = dlopen("libminecraftpe.so", RTLD_LAZY);
    if (!lib) return -1;

    void **vtable_obj = (void **)dlsym(lib, vtable_sym);
    if (!vtable_obj) {
        dlclose(lib);
        LOGE("vtable","get vtable_obj fail");
        return -1;
    }

    void **vtable = vtable_obj + 2;
    void *needle = dlsym(lib, func_sym);
    if (!needle) {
        dlclose(lib);
        LOGE("get func fail");
        return -1;
    }

    for (int i = 0; i < max_funcs; ++i) {
        if (vtable[i] == needle) {
            dlclose(lib);
            return i;          
        }
    }
    dlclose(lib);
    LOGE("can't get index");
    return -1;
}


void* GetVtableFunc(const char *vtable_sym, int index) {
    void *handle = dlopen("libminecraftpe.so", RTLD_LAZY);
    void **vtable = (void **)dlsym(handle, vtable_sym);
    if (!vtable) {
        dlclose(handle);
        return nullptr;
    }
    uintptr_t raw = (uintptr_t)vtable[index];
    if (!raw) {
        dlclose(handle);
        return nullptr;
    }
    dlclose(handle);
    return (void *)raw;
}

void Vtablehook(const char *vtable_sym, int index, void *replace, void **result){
    void *handle = dlopen("libminecraftpe.so", RTLD_LAZY);
    void **vtable = (void **)dlsym(handle, vtable_sym);
    void** target_addr = &vtable[index];
    if (!vtable) { 
        dlclose(handle); 
        return; 
    }
    size_t page_size = sysconf(_SC_PAGESIZE);
    uintptr_t page_start = (uintptr_t)target_addr & ~(page_size - 1);
    if (mprotect((void*)page_start, page_size, PROT_READ | PROT_WRITE) == 0) {
        *result = vtable[index];
	    vtable[index] = replace;
    }
    dlclose(handle);
    return;
}

