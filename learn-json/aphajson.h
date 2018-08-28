#ifndef APHAJSON_H__
#define APHAJSON_H__

#include <stddef.h>

// 解析出错的话返回的类型是APHA_NULL
typedef enum { APHA_NULL, APHA_FALSE, APHA_TRUE, APHA_NUMBER, APHA_STRING} apha_type; 

typedef struct {
    union {
        struct { char* str; size_t len;}s;
        double number;
    }u;
    apha_type type;
}apha_value;

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
    APHA_PARSE_INVALID_UNICODE_SURROGATE
};

#define apha_init(v) do { (v)->type = APHA_NULL; }while(0)

int apha_parse(apha_value* v, const char* json);

void apha_free(apha_value* v);

apha_type apha_get_type(apha_value* v);
double apha_get_number(apha_value* v);

const char* apha_get_string(const apha_value* v);
void apha_set_string(apha_value* v, const char* str, size_t len);

size_t apha_get_string_length(const apha_value* v);
#endif /* LIGHTJSON_H__ */ 