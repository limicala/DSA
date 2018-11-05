#ifndef APHAJSON_H__
#define APHAJSON_H__

#include <stddef.h>

// 解析出错的话返回的类型是APHA_NULL
typedef enum { APHA_NULL, APHA_FALSE, APHA_TRUE, APHA_NUMBER, APHA_STRING, APHA_ARRAY, APHA_OBJECT } apha_type; 

typedef struct apha_value apha_value;
typedef struct apha_member apha_member;

struct apha_value{
    union {
        struct { apha_member* m; size_t size; }o;
        struct { apha_value* e; size_t size; }a;
        struct { char* str; size_t len;}s;
        double number;
    }u;
    apha_type type;
};

struct apha_member
{
    char* k; int klen;
    apha_value v;
};

enum {
    APHA_PARSE_OK = 0,
    APHA_PARSE_EXCEPT_VALUE,     // 只有空格
    APHA_PARSE_INVALID_VALUE,    // 不是字面值
    APHA_PARSE_ROOT_NOT_SINGULAR,// 在一个值之后，还有除空格之外的别的字符
    APHA_PARSE_NUMBER_TOO_BIG,   // 不在范围内
    APHA_PARSE_MISS_QUOTATION_MARK,
    APHA_PARSE_INVALID_STRING_ESCAPE,
    APHA_PARSE_INVALID_STRING_CHAR,
    APHA_PARSE_INVALID_UNICODE_HEX,
    APHA_PARSE_INVALID_UNICODE_SURROGATE,
    APHA_PARSE_MISS_COMMA_OR_SQUARE_BRACKET
};

#define apha_init(v) do { (v)->type = APHA_NULL; }while(0)

int apha_parse(apha_value* v, const char* json);

void apha_free(apha_value* v);

apha_type apha_get_type(apha_value* v);
double apha_get_number(apha_value* v);

const char* apha_get_string(const apha_value* v);
void apha_set_string(apha_value* v, const char* str, size_t len);

size_t apha_get_string_length(const apha_value* v);

size_t apha_get_array_size(const apha_value* v);
apha_value* apha_get_array_element(const apha_value* v, size_t index);

size_t apha_get_object_size(const apha_value* v);
const char* apha_get_object_key(const apha_value* v, int index);
size_t apha_get_object_key_length(const apha_value* v, int index);
apha_value* apha_get_object_value(const apha_value* v, int index);
#endif /* LIGHTJSON_H__ */ 