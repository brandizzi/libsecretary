#include <secretary/parser.h>
#include <secretary/util.h>

#define BUFFER_SIZE 2048
#define TASK_HAS_PROJECT 0x1
#define TASK_IS_SCHEDULED 0x2
#define TASK_IS_DONE 0x4

// Parser implementations
static Secretary *parser_v1_1(FILE *file) {
    Secretary *secretary  = secretary_new();
    int project_count = getw(file);
    for (int i = 0; i < project_count; i++) {
        //int project_properties = 
            getw(file);
        char *name = util_read_string(file);
        secretary_start(secretary, name);
        free(name);
    }

    int task_count = getw(file);
    for (int i = 0; i < task_count; i++) {
        int properties = getw(file);
        char *description = util_read_string(file);
        Task *task = secretary_appoint(secretary, description);
        free(description);
        if (properties & TASK_HAS_PROJECT) {
            char *name  = util_read_string(file);
            Project *project = secretary_get_project(secretary, name);
            free(name);
            secretary_move(secretary, task, project);
        }
        if (properties & TASK_IS_SCHEDULED) {
            struct tm date;
            fread(&date, sizeof(date), 1, file);
            secretary_schedule(secretary, task, date);
        }
        if (properties & TASK_IS_DONE) {
            secretary_do(secretary, task);
        }
    }
    return secretary;
}

typedef struct {
    int major_version;
    int minor_version;
    ParserFunction parser;
} ParserRow;

// Parser retriever
ParserFunction parser_get_reader(int major_version, int minor_version) {
    static ParserRow parsers[] = {
        {1, 1, parser_v1_1}
    };
    for (int i = 0; i < sizeof(parsers)/sizeof(ParserRow); i++) {
        if (parsers[i].major_version == major_version && 
            parsers[i].minor_version == minor_version) {
            return parsers[i].parser;
        }
    }
    return NULL;
}
