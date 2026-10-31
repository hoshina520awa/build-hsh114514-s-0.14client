#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <dirent.h>

typedef unsigned long int ADDRESS;
typedef char PACKAGENAME;

unsigned int ipid;
unsigned int getPID(char PackageName[64]);

int WriteAddress_HEX(const PACKAGENAME *bm, ADDRESS addr, const char *hexData);
int WriteAddress_DWORD(const PACKAGENAME *bm, ADDRESS addr, int value);
int WriteAddress_FLOAT(const PACKAGENAME *bm, ADDRESS addr, float value);

unsigned long get_module_base(unsigned int pid, const char* module_name) {
    char mapPath[1024];
    char mapLine[1024];
    unsigned long addr = 0;  
    sprintf(mapPath, "/proc/%u/maps", pid);

    FILE *fp = fopen(mapPath, "r");
    if (!fp) {
        return (unsigned long)-1;  
    }

    while (fgets(mapLine, sizeof(mapLine), fp)) {
        if (strstr(mapLine, module_name)) {
            if (sscanf(mapLine, "%lx", &addr) == 1) {  
                break;  
            }
        }
    }

    fclose(fp);
    return addr;
}

unsigned int getPID(PACKAGENAME *PackageName) {
    unsigned int pid = 0;
    char cmdline[1024];
    char cmdname[1024];
    DIR *dir = opendir("/proc");
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        unsigned int id = atoi(entry->d_name);
        if (id != 0) {
            snprintf(cmdline, sizeof(cmdline), "/proc/%u/cmdline", id);
            FILE *fp = fopen(cmdline, "r");
            if (fp != nullptr) {
                fgets(cmdname, sizeof(cmdname), fp);
                fclose(fp);
                if (strcmp(cmdname, PackageName) == 0) {
                    pid = id;
                    break;
                }
            }
        }
    }
    closedir(dir);
    return pid;
}


static int mem_patch(const void *addr, const void *data, size_t len)
{
    long page = sysconf(_SC_PAGESIZE);
    uintptr_t start = (uintptr_t)addr & ~(page - 1);
    size_t  len2 = ((uintptr_t)addr + len - start + page - 1) & ~(page - 1);

    if (mprotect((void *)start, len2, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
        return -1;
    }

    int fd = open("/proc/self/mem", O_RDWR);
    if (fd < 0) {
        return -1;
    }

    if (pwrite64(fd, data, len, (uintptr_t)addr) != (ssize_t)len) {
        close(fd);
        return -1;
    }

    unsigned char verify[len];
    if (pread64(fd, verify, len, (uintptr_t)addr) != (ssize_t)len ||
        memcmp(verify, data, len) != 0) {
        close(fd);
        return -1;
    }

    close(fd);
    msync((void *)start, len2, MS_INVALIDATE);
    __builtin___clear_cache((char *)addr, (char *)addr + len);
    return 0;
}

int WriteAddress_HEX(const PACKAGENAME *bm, ADDRESS addr, const char *hexData)
{
    if (!bm || !hexData) return -1;
    size_t hexLen = strlen(hexData);
    if (hexLen == 0 || hexLen % 2 != 0) {
        return -1;
    }
    size_t byteLen = hexLen / 2;
    auto *bin = (unsigned char *)malloc(byteLen);
    if (!bin) return -1;

    for (size_t i = 0; i < byteLen; ++i)
        if (sscanf(hexData + i * 2, "%2hhx", &bin[i]) != 1) {
            free(bin);
            return -1;
        }

    int rc = mem_patch((void *)addr, bin, byteLen);
    if (rc == 0) {
        char hexStr[byteLen * 2 + 1];
        for (size_t i = 0; i < byteLen; ++i)
            sprintf(hexStr + i * 2, "%02X", bin[i]);
    }
    free(bin);
    return rc;
}

int WriteAddress_DWORD(const PACKAGENAME *bm, ADDRESS addr, int value)
{
    if (!bm) return -1;
    int rc = mem_patch((void *)addr, &value, sizeof(value));
    if (rc == 0)
    return rc;
}

int WriteAddress_FLOAT(const PACKAGENAME *bm, ADDRESS addr, float value)
{
    if (!bm) return -1;
    union { float f; uint32_t u; } tmp = { .f = value };
    int rc = mem_patch((void *)addr, &tmp, sizeof(tmp));
    if (rc == 0)
    return rc;
}

