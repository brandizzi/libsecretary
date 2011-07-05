
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
#ifndef _SECRETARY_PARSER_H
#  define _SECRETARY_PARSER_H

#include <secretary/secretary.h>
#include <stdio.h>

#define PARSER_LATEST_MAJOR_VERSION 1
#define PARSER_LATEST_MINOR_VERSION 2

typedef Secretary* (*ParserReaderFunction)(FILE *file);
typedef void (*ParserWriterFunction)(Secretary *secretary, FILE *file);

ParserReaderFunction parser_get_reader(int major_version, int minor_version);
ParserWriterFunction parser_get_writer(int major_version, int minor_version);

#endif
