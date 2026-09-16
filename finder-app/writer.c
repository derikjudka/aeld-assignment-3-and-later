#include <stdio.h>
#include <syslog.h>
#include <string.h>

int main(int argc, char *argv[])
{
    openlog(NULL, 0, LOG_USER);
    if (argc < 3)
    {
        syslog(LOG_ERR, "ERROR: Invalid number of arguments [Received: %d] [Expected: 2]", argc-1);
        return 1;
    }

    FILE *file = fopen(argv[1], "wb");
    size_t str_len = strlen(argv[2]);
    if (file == NULL)
    {
        syslog(LOG_ERR, "ERROR: Could not open file %s", argv[1]);
        return 1;
    }

    if (fwrite(argv[2], sizeof(char), str_len, file) == str_len)
    {
        syslog(LOG_DEBUG, "Writing %s to %s", argv[2], argv[1]);
    }
    else
    {
        syslog(LOG_ERR, "ERROR: Failed to write %s to %s", argv[2], argv[1]);
        return 1;
    }

    fclose(file);
    return 0;
}
