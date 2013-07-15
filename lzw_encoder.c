#include "lzw_encoder.h"
#include <stdlib.h>

typedef struct DictEntry {
	unsigned code;
	struct DictEntry* children[];
} DictEntry;

typedef struct {
	size_t n;
	size_t code_size;
	unsigned code_end, code_clear;
	unsigned last_code;
	DictEntry* contents;
} Dictionary;

static void Dict_deleteEntry(size_t n, DictEntry* de)
{
	for(size_t i = 0;  i < n;  i++)
		if(de->children[i])
			Dict_deleteEntry(n, de->children[i]);
	
	free(de);
}

static void Dict_clear(Dictionary* dict)
{
	Dict_deleteEntry(dict->n, dict->contents);
}

/*static void Dict_delete(Dictionary* dict)
{
	Dict_clear(dict);
	free(dict);
}*/

static DictEntry* Dict_createEntry(size_t n, unsigned code)
{
	DictEntry* de;
	
	de = malloc(sizeof(*de) + n * sizeof(*de->children));
	
	de->code = code;
	
	while(n)
		de->children[--n] = NULL;
	
	return de;
}

static void Dict_init(Dictionary* dict, size_t n, size_t initial_code_size)
{ 
	dict->n = n;
	
	dict->contents = Dict_createEntry(n, (unsigned)-1);
	for(size_t i = 0;  i < n;  i++)
		dict->contents->children[i] = Dict_createEntry(n, i);
	
	dict->code_end = (dict->code_clear = n) + 1;
	dict->last_code = n + 1;
	dict->code_size = initial_code_size + 1;
}

/*static Dictionary* Dict_create(size_t n)
{
	Dictionary* dict;
	
	dict = malloc(sizeof(*dict));
	
	Dict_init(dict, n);
	
	return dict;
}*/

static void Dict_addEntry(Dictionary* dict, DictEntry* where, Byte b)
{
	where->children[b] = Dict_createEntry(dict->n, ++dict->last_code);
	
	if(dict->last_code == (unsigned)1<<dict->code_size)
		dict->code_size++;
}

static size_t min(size_t a, size_t b)
{
	return a<b ? a : b;
}

static void writeCode(unsigned code, size_t code_size, size_t* off, Byte* bytes)
{ 
	while(code_size) {
		size_t i = *off / BYTE_SZ,
		       j = *off % BYTE_SZ;
		size_t m = min(code_size, BYTE_SZ - j);
		if(bytes) {
			if(!j)
				bytes[i] = 0;
			bytes[i] |= (code << j) & BYTE_MAX;
		}
		code >>= m;
		code_size -= m;
		*off += m;
	}
}

size_t lzw_encode(size_t initial_code_size, size_t len, const Byte* data, Byte* out_bytes)
{
	Dictionary dict;
	DictEntry* de;
	size_t i;
	size_t out_off;
	Byte b;
	
	Dict_init(&dict, 1<<initial_code_size, initial_code_size);
	de = dict.contents;
	i = 0;
	out_off = 0;
	
	writeCode(dict.code_clear, dict.code_size, &out_off, out_bytes);
	while(i < len) {
		b = data[i];
		if(de->children[b]) {
			de = de->children[b];
			i++;
		}
		else {
			writeCode(de->code, dict.code_size, &out_off, out_bytes);
			Dict_addEntry(&dict, de, b);
			de = dict.contents;
		}
	}
	writeCode(de->code, dict.code_size, &out_off, out_bytes);
	writeCode(dict.code_end, dict.code_size, &out_off, out_bytes);
	
	Dict_clear(&dict);
	
	return (out_off + BYTE_SZ - 1) / BYTE_SZ;
}
