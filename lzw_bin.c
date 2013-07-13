#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>

typedef unsigned char Byte;
#define  BYTE_MAX  UCHAR_MAX
#define  BYTE_SZ   CHAR_BIT

typedef struct DictEntry {
	unsigned code;
	struct DictEntry* children[];
} DictEntry;

typedef struct {
	unsigned n;
	unsigned code_end, code_clear;
	unsigned last_code;
	unsigned code_size;
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

void Dict_init(Dictionary* dict, unsigned n, unsigned initial_code_size)
{ 
	dict->n = n;
	
	dict->contents = Dict_createEntry(n, EOF);
	for(unsigned i = 0;  i < n;  i++)
		dict->contents->children[i] = Dict_createEntry(n, i);
	
	dict->code_end = (dict->code_clear = n) + 1;
	dict->last_code = n + 1;
	dict->code_size = initial_code_size + 1;
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
	
	if(dict->last_code == (unsigned)1<<dict->code_size)
		dict->code_size++;
}

typedef struct {
	Byte b;
	unsigned off;
	FILE* file;
} Output;

void writeCode(Output* out, unsigned code, unsigned code_size)
{ 
	out->b |= (code << out->off) & BYTE_MAX;
	
	while(out->off + code_size >= BYTE_SZ) {
		putc(out->b, out->file);
		code >>= BYTE_SZ - out->off;
		code_size -= BYTE_SZ - out->off;
		out->off = 0;
		out->b = code & BYTE_MAX;
	}
	
	out->off += code_size;
}

void flushOutput(Output* out)
{
	if(out->off)
		putc(out->b, out->file);
	fflush(out->file);
}

int main(void)
{
	Dictionary dict;
	DictEntry* de;
	int c;
	Byte b;
	Output out = {.b = 0,  .off = 0,  .file = stdout};
	
	Dict_init(&dict, BYTE_MAX+1, BYTE_SZ);
	de = (DictEntry*)( (char*)&dict.contents - offsetof(DictEntry,children) );
	b = 0;
	
	writeCode(&out, dict.code_clear, dict.code_size);
	do {
		if(de->children[b]) {
			de = de->children[b];
			c = getchar();
			b = (Byte) c;
		}
		else {
			Dict_addEntry(&dict, de, b);
			writeCode(&out, de->code, dict.code_size);
			de = dict.contents;
		}
	} while(c != EOF);
	writeCode(&out, de->code, dict.code_size);
	writeCode(&out, dict.code_end, dict.code_size);
	flushOutput(&out);
	
	Dict_clear(&dict);
	
	return EXIT_SUCCESS;
}
