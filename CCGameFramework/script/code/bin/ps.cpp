#include "/include/string"
#include "/include/shell"
int main(int argc, char** argv) {
    if (argc == 1) { // ps
        shell("cat /sys/ps");
    }
    else if (argc == 2) { // ps
        if (strcmp(argv[1], "tree") == 0) {
            shell("cat /sys/pstree");
        }
        else {
            shell("cat /sys/ps");
        }
    }
    else {
        set_fg(240, 0, 0);
        put_string("[Error] Invalid argument.");
        restore_fg();
    }
    return 0;
}