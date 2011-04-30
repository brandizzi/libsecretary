#include <secretary/parser.h>
#include <secretary/util.h>

#define BUFFER_SIZE 2048
#define TASK_HAS_PROJECT 0x1
#define TASK_IS_SCHEDULED 0x2
#define TASK_IS_DONE 0x4

// Parser implementations
static Secretary *parser_reader_v1_1(FILE *file) {
    Secretary *secretary  = secretary_new();
    int project_count = getw(file);
    for (int i = 0; i < project_count; i++) {
        //int project_properties = 
            getw(file);
        char *name = util_read_string(file);
        secretary_create_project(secretary, name);
        free(name);
    }

    int task_count = getw(file);
    for (int i = 0; i < task_count; i++) {
        int properties = getw(file);
        char *description = util_read_string(file);
        Task *task = secretary_create_task(secretary, description);
        free(description);
        if (properties & TASK_HAS_PROJECT) {
            char *name  = util_read_string(file);
            Project *project = secretary_get_project(secretary, name);
            free(name);
            secretary_move_to_project(secretary, task, project);
        }
        if (properties & TASK_IS_SCHEDULED) {
            struct tm date;
            fread(&date, sizeof(date), 1, file);
            secretary_schedule(secretary, task, date);
        }
        if (properties & TASK_IS_DONE) {
            secretary_mark_task_as_done(secretary, task);
        }
    }
    return secretary;
}

static void parser_writer_v1_1(Secretary *secretary, FILE *file) {
    putw(secretary_count_projects(secretary), file);
    for (int i = 0; i < secretary_count_projects(secretary); i++) {
        Project *project = secretary_get_nth_project(secretary, i);
        int mask = 0;
        putw(mask, file);
        util_write_string(file, project_get_name(project));
    }
    putw(secretary_count_tasks(secretary), file);
    for (int i = 0; i < secretary_count_tasks(secretary); i++) {
        Task *task = secretary_get_nth_task(secretary, i);
        int mask = 0;
        if (task_get_project(task)) {
            mask |= TASK_HAS_PROJECT;
        }
        if (task_is_scheduled(task)) {
            mask |= TASK_IS_SCHEDULED;
        }
        if (task_is_done(task)) {
            mask |= TASK_IS_DONE;
        }
        putw(mask, file);
        util_write_string(file, task_get_description(task));

        if (mask & TASK_HAS_PROJECT) {
            util_write_string(file, 
                    project_get_name(task_get_project(task)));
        }
        if (mask & TASK_IS_SCHEDULED) {
            struct tm date = task_get_scheduled_date(task);
            fwrite(&date, sizeof(date), 1, file);
        }
    }
}
// Parser retriever
typedef struct {
    int major_version;
    int minor_version;
    ParserReaderFunction reader;
    ParserWriterFunction writer;
} ParserRow;

#define PARSER_ROW(major_version, minor_version) { \
    major_version, minor_version, \
    parser_reader_v ## major_version ## _ ## minor_version, \
    parser_writer_v ## major_version ## _ ## minor_version \
}

// Just one search function, since it is non-trivial code
#define PARSER_READER 0
#define PARSER_WRITER 1
void *parser_get(int major_version, int minor_version, int what) {
    static ParserRow parsers[] = {
        PARSER_ROW(1, 1)
    };
    for (int i = 0; i < sizeof(parsers)/sizeof(ParserRow); i++) {
        if (parsers[i].major_version == major_version && 
            parsers[i].minor_version == minor_version) {
            switch (what) {
            case PARSER_READER:
                return parsers[i].reader;
            case PARSER_WRITER:
                return parsers[i].writer;
            }
        }
    }
    return NULL;
}

ParserReaderFunction parser_get_reader(int major_version, int minor_version) {
    return (ParserReaderFunction) 
        parser_get(major_version, minor_version, PARSER_READER);
}

ParserWriterFunction parser_get_writer(int major_version, int minor_version) {
    return (ParserWriterFunction) 
        parser_get(major_version, minor_version, PARSER_WRITER);

}
