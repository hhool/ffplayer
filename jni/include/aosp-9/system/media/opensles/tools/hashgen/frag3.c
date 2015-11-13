
extern const struct SLInterfaceID_ SL_IID_array[MPH_MAX];
extern const char * const interface_names[MPH_MAX];

int main(int argc, char **argv)
{
    int i;
    for (i = 0; i <= MAX_HASH_VALUE; ++i) {
        const char *x = wordlist[i];
        if (!x) {
            printf("        -1");
        } else {
            const struct SLInterfaceID_ *xx = SL_IID_array;
            unsigned MPH;
            for (MPH = 0; MPH < MPH_MAX; ++MPH, ++xx) {
                if (!memcmp(x, xx, 16)) {
                    printf("        MPH_%s", interface_names[MPH]);
                    goto out;
                }
            }
            printf("        (-1)");
out:
            ;
        }
        if (i < MAX_HASH_VALUE)
            printf(",");
        printf("\n");
    }
    return EXIT_SUCCESS;
}
