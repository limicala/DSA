#ifndef APHAJSON_H__
#define APHAJSON_H__

// 解析出错的话返回的类型是APHA_NULL
typedef enum { APHA_NULL, APHA_FALSE, APHA_TRUE} apha_type; 

typedef struct {
    apha_type type;
}apha_value;

typedef struct {
    const char* json;
}apha_context;

enum {
    APHA_PARSE_OK = 0,
    APHA_PARSE_EXCEPT_VALUE,     // 只有空格
    APHA_PARSE_INVALID_VALUE,    // 不是字面值
    APHA_PARSE_ROOT_NOT_SINGULAR // 在一个值之后，还有除空格之外的别的字符
};

int apha_parse(apha_value* v, const char* json);

apha_type apha_get_type(apha_value* v);

#endif /* LIGHTJSON_H__ */ 