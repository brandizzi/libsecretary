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
#include "config.h" 

#include <secretary/parser.h>
#include <secretary/util.h>
#include <secretary/_internal/secretary.h>

#include "string.h"

#define BUFFER_SIZE 2048
#define TASK_HAS_PROJECT 0x1
#define TASK_IS_SCHEDULED 0x2
#define TASK_IS_DONE 0x4
// Used by v1.2
#define TASK_IS_ARCHIVED 0x8

// Parser implementations
static SctSecretary *parser_reader_v1_1(FILE *file) {
    SctSecretary *secretary  = sct_secretary_new();
    int sct_project_count = getw(file);
    for (int i = 0; i < sct_project_count; i++) {
        //int sct_project_properties = 
            getw(file);
        char *name = sct_util_read_string(file);
        sct_secretary_create_project(secretary, name);
        free(name);
    }

    int sct_task_count = getw(file);
    for (int i = 0; i < sct_task_count; i++) {
        int properties = getw(file);
        char *description = sct_util_read_string(file);
        SctTask *task = sct_secretary_create_task(secretary, description);
        free(description);
        if (properties & TASK_HAS_PROJECT) {
            char *name  = sct_util_read_string(file);
            SctProject *project = sct_secretary_get_project(secretary, name);
            free(name);
            sct_project_add_task(project, task);
        }
        if (properties & TASK_IS_SCHEDULED) {
            struct tm date;
            if (fread(&date, sizeof(struct tm), 1, file) < 1) goto ERROR;
            sct_task_schedule(task, mktime(&date));
        }
        if (properties & TASK_IS_DONE) {
            sct_task_mark_as_done(task);
        }
    }
    _secretary_update_sublists(secretary);
    return secretary;
    ERROR:
    sct_secretary_free(secretary);
    return NULL;
}

static void parser_writer_v1_1(SctSecretary *secretary, FILE *file) {
    // Saving version
    putc(1, file);
    putc(1, file);
    // Going ahead
    putw(sct_secretary_count_projects(secretary), file);
    for (int i = 0; i < sct_secretary_count_projects(secretary); i++) {
        SctProject *project = sct_secretary_get_nth_project(secretary, i);
        int mask = 0;
        putw(mask, file);
        sct_util_write_string(file, sct_project_get_name(project));
    }
    putw(sct_secretary_count_all_tasks(secretary), file);
    for (int i = 0; i < sct_secretary_count_all_tasks(secretary); i++) {
        SctTask *task = sct_secretary_get_nth_task(secretary, i);
        int mask = 0;
        if (sct_task_get_project(task)) {
            mask |= TASK_HAS_PROJECT;
        }
        if (sct_task_is_scheduled(task)) {
            mask |= TASK_IS_SCHEDULED;
        }
        if (sct_task_is_done(task)) {
            mask |= TASK_IS_DONE;
        }
        putw(mask, file);
        sct_util_write_string(file, sct_task_get_description(task));

        if (mask & TASK_HAS_PROJECT) {
            sct_util_write_string(file, 
                    sct_project_get_name(sct_task_get_project(task)));
        }
        if (mask & TASK_IS_SCHEDULED) {
            time_t date = sct_task_get_scheduled_date(task);
            fwrite(gmtime(&date), sizeof(struct tm), 1, file);
        }
    }
}

/**
 * This parser version supports archival.
 */
static SctSecretary *parser_reader_v1_2(FILE *file) {
    SctSecretary *secretary  = sct_secretary_new();
    int sct_project_count = getw(file);
    for (int i = 0; i < sct_project_count; i++) {
        //int sct_project_properties = 
            getw(file);
        char *name = sct_util_read_string(file);
        sct_secretary_create_project(secretary, name);
        free(name);
    }

    int sct_task_count = getw(file);
    for (int i = 0; i < sct_task_count; i++) {
        int properties = getw(file);
        char *description = sct_util_read_string(file);
        SctTask *task = sct_secretary_create_task(secretary, description);
        free(description);
        if (properties & TASK_HAS_PROJECT) {
            char *name  = sct_util_read_string(file);
            SctProject *project = sct_secretary_get_project(secretary, name);
            free(name);
            sct_project_add_task(project, task);
        }
        if (properties & TASK_IS_SCHEDULED) {
            struct tm date;
            if (fread(&date, sizeof(struct tm), 1, file) < 1) goto ERROR;
            sct_task_schedule(task, mktime(&date));
        }
        if (properties & TASK_IS_DONE) {
            sct_task_mark_as_done(task);
        }
        if (properties & TASK_IS_ARCHIVED) {
            sct_task_archive(task);
        }
    }
    _secretary_update_sublists(secretary);
    return secretary;
    ERROR:
    sct_secretary_free(secretary);
    return NULL;
}

/**
 * This parser version supports archival.
 */

static void parser_writer_v1_2(SctSecretary *secretary, FILE *file) {
    // Saving version
    putc(1, file);
    putc(2, file);
    // Going ahead
    putw(sct_secretary_count_projects(secretary), file);
    for (int i = 0; i < sct_secretary_count_projects(secretary); i++) {
        SctProject *project = sct_secretary_get_nth_project(secretary, i);
        int mask = 0;
        putw(mask, file);
        sct_util_write_string(file, sct_project_get_name(project));
    }
    putw(sct_secretary_count_all_tasks(secretary), file);
    for (int i = 0; i < sct_secretary_count_all_tasks(secretary); i++) {
        SctTask *task = sct_secretary_get_nth_task(secretary, i);
        int mask = 0;
        if (sct_task_get_project(task)) {
            mask |= TASK_HAS_PROJECT;
        }
        if (sct_task_is_scheduled(task)) {
            mask |= TASK_IS_SCHEDULED;
        }
        if (sct_task_is_done(task)) {
            mask |= TASK_IS_DONE;
        }
        if (sct_task_is_archived(task)) {
            mask |= TASK_IS_ARCHIVED;
        }

        putw(mask, file);
        sct_util_write_string(file, sct_task_get_description(task));

        if (mask & TASK_HAS_PROJECT) {
            sct_util_write_string(file, 
                    sct_project_get_name(sct_task_get_project(task)));
        }
        if (mask & TASK_IS_SCHEDULED) {
            time_t date = sct_task_get_scheduled_date(task);
            fwrite(gmtime(&date), sizeof(struct tm), 1, file);
        }
    }
}

/**
 * This parser saves time_t instead of struct tm.
 */
static SctSecretary *parser_reader_v1_3(FILE *file) {
    SctSecretary *secretary  = sct_secretary_new();
    int sct_project_count = getw(file);
    for (int i = 0; i < sct_project_count; i++) {
        //int sct_project_properties = 
            getw(file);
        char *name = sct_util_read_string(file);
        sct_secretary_create_project(secretary, name);
        free(name);
    }

    int sct_task_count = getw(file);
    for (int i = 0; i < sct_task_count; i++) {
        int properties = getw(file);
        int number = getw(file);
        time_t creation_date = sct_util_read_number(file, 8);
        char *description = sct_util_read_string(file);
        SctTask *task = sct_task_new(description);
        free(description);
        task->number = number;
        task->created_at = creation_date;
        if (properties & TASK_HAS_PROJECT) {
            char *name  = sct_util_read_string(file);
            SctProject *project = sct_secretary_get_project(secretary, name);
            free(name);
            sct_project_add_task(project, task);
        }
        if (properties & TASK_IS_SCHEDULED) {
            time_t date = sct_util_read_number(file, 8);
            sct_task_schedule(task, date);
        }
        if (properties & TASK_IS_DONE) {
            sct_task_mark_as_done(task);
        }
        if (properties & TASK_IS_ARCHIVED) {
            sct_task_archive(task);
        }
        _secretary_add_task(secretary, task);
    }
    _secretary_update_sublists(secretary);
    return secretary;
    ERROR:
    sct_secretary_free(secretary);
    return NULL;
}

/**
 * This parser saves time_t instead of struct tm.
 */

static void parser_writer_v1_3(SctSecretary *secretary, FILE *file) {
    // Saving version
    putc(1, file);
    putc(3, file);
    // Going ahead
    putw(sct_secretary_count_projects(secretary), file);
    for (int i = 0; i < sct_secretary_count_projects(secretary); i++) {
        SctProject *project = sct_secretary_get_nth_project(secretary, i);
        int mask = 0;
        putw(mask, file);
        sct_util_write_string(file, sct_project_get_name(project));
    }
    putw(sct_secretary_count_all_tasks(secretary), file);
    for (int i = 0; i < sct_secretary_count_all_tasks(secretary); i++) {
        SctTask *task = sct_secretary_get_nth_task(secretary, i);
        int mask = 0;
        if (sct_task_get_project(task)) {
            mask |= TASK_HAS_PROJECT;
        }
        if (sct_task_is_scheduled(task)) {
            mask |= TASK_IS_SCHEDULED;
        }
        if (sct_task_is_done(task)) {
            mask |= TASK_IS_DONE;
        }
        if (sct_task_is_archived(task)) {
            mask |= TASK_IS_ARCHIVED;
        }

        putw(mask, file);
        putw(task->number, file);
        time_t date = sct_task_get_creation_date(task);

        sct_util_write_number(file, date, 8);

        sct_util_write_string(file, sct_task_get_description(task));

        if (mask & TASK_HAS_PROJECT) {
            sct_util_write_string(file, 
                    sct_project_get_name(sct_task_get_project(task)));
        }
        if (mask & TASK_IS_SCHEDULED) {
            time_t date = sct_task_get_scheduled_date(task);
            sct_util_write_number(file, date, 8);
        }
    }
}

// Parser retriever
typedef struct {
    int major_version;
    int minor_version;
    SctParserReaderFunction reader;
    SctParserWriterFunction writer;
} ParserRow;

#define SCT_PARSER_ROW(major_version, minor_version) { \
    major_version, minor_version, \
    parser_reader_v ## major_version ## _ ## minor_version, \
    parser_writer_v ## major_version ## _ ## minor_version \
}
static ParserRow parsers[] = {
    SCT_PARSER_ROW(1, 1),
    SCT_PARSER_ROW(1, 2),
    SCT_PARSER_ROW(1, 3)
};

// Just one search function, since it is non-trivial code
#define SCT_PARSER_READER 0
#define SCT_PARSER_WRITER 1

static void *parser_get(int major_version, int minor_version, int what) {

    for (int i = 0; i < sizeof(parsers)/sizeof(ParserRow); i++) {
        if (parsers[i].major_version == major_version && 
            parsers[i].minor_version == minor_version) {
            switch (what) {
            case SCT_PARSER_READER:
                return parsers[i].reader;
            case SCT_PARSER_WRITER:
                return parsers[i].writer;
            }
        }
    }
    return NULL;
}

SctParserReaderFunction parser_get_reader(int major_version, int minor_version) {
    return (SctParserReaderFunction) 
        parser_get(major_version, minor_version, SCT_PARSER_READER);
}

SctParserWriterFunction parser_get_writer(int major_version, int minor_version) {
    return (SctParserWriterFunction) 
        parser_get(major_version, minor_version, SCT_PARSER_WRITER);

}
