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
#include <secretary/notebook.h>
#include <secretary/util.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 2048
#define TASK_HAS_PROJECT 0x1
#define TASK_IS_SCHEDULED 0x2
#define TASK_IS_DONE 0x4

Notebook *notebook_new(const char *filename) {
    Notebook *notebook = malloc(sizeof(Notebook));
    notebook->filename = util_copy_string(filename);

    FILE *file = fopen(filename, "r");
    if (file) {
        notebook->major_version = getc(file);
        notebook->minor_version = getc(file);
        ParserReaderFunction reader = parser_get_reader(
                notebook->major_version, notebook->minor_version);
        notebook->secretary = reader(file);
    } else {
        notebook->secretary = secretary_new();

    }
    notebook->major_version = PARSER_LATEST_MAJOR_VERSION;
    notebook->minor_version = PARSER_LATEST_MINOR_VERSION;
    return notebook;
}

Secretary *notebook_get_secretary (Notebook *notebook) {
    return notebook->secretary;
}

void notebook_save(Notebook *notebook) {
    FILE *file = fopen(notebook->filename, "w");
    ParserWriterFunction writer = parser_get_writer(
            notebook->major_version, notebook->minor_version);
    writer(notebook->secretary, file);
    fclose(file);
}

void notebook_free(Notebook *notebook) {
    secretary_free(notebook_get_secretary(notebook));
    free(notebook->filename);
    free(notebook);
}


