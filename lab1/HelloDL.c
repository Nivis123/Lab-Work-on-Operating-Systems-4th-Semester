#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main(void) {
    printf("Hello, world!\n");

    void *handle = dlopen("./libdynamicdl_lib.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Error: %s\n", dlerror());
        return EXIT_FAILURE;
    }

    dlerror();

    void (*hello_from_dyn_runtime_lib)() = dlsym(handle, "hello_from_dyn_runtime_lib");

    char *error = dlerror();
    if (error) {
        fprintf(stderr, "Error: %s\n", error);
        dlclose(handle);
        return EXIT_FAILURE;
    }

    hello_from_dyn_runtime_lib();

    dlclose(handle);

    return EXIT_SUCCESS;
}
