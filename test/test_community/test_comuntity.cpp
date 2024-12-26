#include "iostream"

using namespace std;

int8_t split_str(const char *line, char result[][32])
{
    if (line[0] != '$')
        return -1;
    uint16_t index = 0;
    uint16_t count = 0;
    uint16_t temp_index = 0;
    for (index = 1; line[index] != '\0'; index++)
    {
        if (line[index] == '=')
        {
            result[count++][temp_index++] = '\0';
            temp_index = 0;
            continue;
        }
        result[count][temp_index++] = line[index];
    }
    result[count][temp_index++] = '\0';
    if (count != 1)
    {
        return -1;
    }
    return count;
}



int main()
{
    char line[512];
    char result[10][32];
    int count = 0;
    count = split_str("wifi_ssid", result);
    if (count <= 0)
    {
        printf("$result=error parse\n");
    }
    printf("conut=%d\n", count);
    printf("result=");
    return 0;
}