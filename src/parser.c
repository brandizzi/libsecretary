
/**
 * libsecretary: a C library for managing to-do lists
 * Copyright (C) 2011  Adam Victor Nazareth Brandizzi <brandizzi@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * You can get the latest version of this file at 
 * http://bitbucket.org/brandizzi/libsecretary/
 */
#include <secretary/parser.h>
#include <secretary/util.h>

#define BUFFER_SIZE 2048
#define TASK_HAS_PROJECT 0x1
#define TASK_IS_SCHEDULED 0x2
#define TASK_IS_DONE 0x4
// Used by v1.2
#define TASK_IS_ARCHIVED 0x8

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
            project_add_task(project, task);
        }
        if (properties & TASK_IS_SCHEDULED) {
            struct tm date;
            fread(&date, sizeof(date), 1, file);
            task_schedule(task, date);
        }
        if (properties & TASK_IS_DONE) {
            task_mark_as_done(task);
        }
    }
    return secretary;
}

static void parser_writer_v1_1(Secretary *secretary, FILE *file) {
    // Saving version
    putc(1, file);
    putc(1, file);
    // Going ahead
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

/**
 * This parser version supports archival.
 */
static Secretary *parser_reader_v1_2(FILE *file) {
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
            project_add_task(project, task);
        }
        if (properties & TASK_IS_SCHEDULED) {
            struct tm date;
            fread(&date, sizeof(date), 1, file);
            task_schedule(task, date);
        }
        if (properties & TASK_IS_DONE) {
            task_mark_as_done(task);
        }
        if (properties & TASK_IS_ARCHIVED) {
            task_archive(task);
        }
    }
    return secretary;
}

/**
 * This parser version supports archival.
 */

static void parser_writer_v1_2(Secretary *secretary, FILE *file) {
    // Saving version
    putc(1, file);
    putc(2, file);
    // Going ahead
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
        if (task_is_archived(task)) {
            mask |= TASK_IS_ARCHIVED;
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
static ParserRow parsers[] = {
    PARSER_ROW(1, 1),
    PARSER_ROW(1, 2)
};

// Just one search function, since it is non-trivial code
#define PARSER_READER 0
#define PARSER_WRITER 1

static void *parser_get(int major_version, int minor_version, int what) {

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
