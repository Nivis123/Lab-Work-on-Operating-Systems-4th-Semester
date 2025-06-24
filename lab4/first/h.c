#include <stdio.h>
#include <stdlib.h>

void env_operations() {
    
    char* env_var = getenv("MY_ENV_VAR");
    if (env_var != NULL) {
        printf("Initial MY_ENV_VAR: %s\n", env_var);
    } else {
        printf("MY_ENV_VAR not found\n");
    }

    setenv("MY_ENV_VAR", "new_value", 1);

    env_var = getenv("MY_ENV_VAR");
    if (env_var != NULL) {
        printf("Updated MY_ENV_VAR: %s\n", env_var);
    } else {
        printf("MY_ENV_VAR not found\n");
    }
}

int main() {
    env_operations();
    return 0;
}
//echo $MY_ENV_VAR
//setenv("MY_VAR", "new_value", 1);
//system("echo $MY_VAR > /tmp/env_update");
//source /tmp/env_update