#include <stdio.h>
#include <dirent.h>
#include <malloc.h>
#include <string.h>
char* html = NULL;

void Append(char** phtml, const char* name)
{
    int oldLen = *phtml == NULL ? 0 : strlen(*phtml); //Tính số ký tự cữ
    (*phtml) = (char*)realloc(*phtml, oldLen + strlen(name) + 1); //Cấp phát thêm
    memset(*phtml + oldLen, 0, strlen(name) + 1); //Điền 0 vào các byte nhớ đc cấp phát thêm
    sprintf(*phtml + oldLen, "%s", name); //Nối xâu
}

int main()
{
    struct dirent **namelist;
    int n;

    n = scandir(".", &namelist, NULL, NULL);

    if (n == -1) {
        printf("Error\n");
    }else
    {
        Append(&html,"<html>\n");
        while (n--) {
            Append(&html,"<a href = \"");
            Append(&html, namelist[n]->d_name);
            if (namelist[n]->d_type == DT_DIR)
            {
                Append(&html,"\"><b>");
                Append(&html, namelist[n]->d_name);
                Append(&html,"</b></a><br>\n");
            }else
            {
                Append(&html,"\"><i>");
                Append(&html, namelist[n]->d_name);
                Append(&html,"</i></a><br>\n");
            }
            free(namelist[n]);
        }
        Append(&html,"</html>\n");
        free(namelist);
        namelist = NULL;
        FILE* f = fopen("scandir.html", "wt");
        fputs(html, f);
        fclose(f);
        free(html);
        html = NULL;
    }
}