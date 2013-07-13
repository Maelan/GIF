#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef unsigned char Byte;
#define  BYTE_MAX  UCHAR_MAX

typedef struct DictEntry {
	unsigned code;
	struct DictEntry* children[];
} DictEntry;

typedef struct {
	unsigned n;
	unsigned last_code;
	DictEntry* contents;
} Dictionary;

void Dict_deleteEntry(unsigned n, DictEntry* de)
{
	for(unsigned i = 0;  i < n;  i++)
		if(de->children[i])
			Dict_deleteEntry(n, de->children[i]);
	
	free(de);
}

void Dict_clear(Dictionary* dict)
{
	Dict_deleteEntry(dict->n, dict->contents);
}

/*void Dict_delete(Dictionary* dict)
{
	Dict_clear(dict);
	free(dict);
}*/

DictEntry* Dict_createEntry(unsigned n, unsigned code)
{
	DictEntry* de;
	
	de = malloc(sizeof(*de) + n * sizeof(*de->children));
	
	de->code = code;
	
	while(n)
		de->children[--n] = NULL;
	
	return de;
}

void Dict_init(Dictionary* dict, unsigned n)
{
	dict->n = n;
	
	dict->contents = Dict_createEntry(n, EOF);
	for(unsigned i = 0;  i < n;  i++)
		dict->contents->children[i] = Dict_createEntry(n, i);
	
	dict->last_code = n-1;
}

/*Dictionary* Dict_create(unsigned n)
{
	Dictionary* dict;
	
	dict = malloc(sizeof(*dict));
	
	Dict_init(dict, n);
	
	return dict;
}*/

void Dict_addEntry(Dictionary* dict, DictEntry* where, Byte b)
{
	where->children[b] = Dict_createEntry(dict->n, ++dict->last_code);
}

void writeCode(unsigned code)
{
	if(code < 128)
		putchar(code);
	else
		printf("<%u>", code);
}

int main(void)
{
	Dictionary dict;
	DictEntry* de;
	int c;
	Byte b;
	
	Dict_init(&dict, BYTE_MAX+1);
	de = dict.contents;
	
	c = getchar();
	while(c != EOF) {
		b = (Byte) c;
		
		if(de->children[b]) {
			de = de->children[b];
			c = getchar();
		}
		else {
			Dict_addEntry(&dict, de, b);
			writeCode(de->code);
			de = dict.contents;
		}
	}
	writeCode(de->code);
	
	Dict_clear(&dict);
	
	return EXIT_SUCCESS;
}
