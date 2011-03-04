#ifndef _SECRETARY_PARSER_H
#  define _SECRETARY_PARSER_H

#include <secretary/secretary.h>
#include <stdio.h>

#define PARSER_LATEST_MAJOR_VERSION 1
#define PARSER_LATEST_MINOR_VERSION 1

typedef Secretary* (*ParserReaderFunction)(FILE *file);
typedef void (*ParserWriterFunction)(Secretary *secretary, FILE *file);

ParserReaderFunction parser_get_reader(int major_version, int minor_version);

#endif
