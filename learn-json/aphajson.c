#include "aphajson.h"
#include "env.h"

#ifndef APHA_STACK_INIT_SIZE
#define APHA_STACK_INIT_SIZE 256
#endif

#define isDigit1To9(ch) ('1' <= ch && ch <= '9')
#define isDigit(ch)     ('0' <= ch && ch <= '9')

#define PUTC(c, ch) do { *(char*)apha_push(c, sizeof(char)) = ch; } while(0)

typedef struct {
    const char* json;
    char* stack;
    size_t size, top;
}apha_context;

static void* apha_push(apha_context* c, size_t size){
    void* addr;
    if(c->size - c->top < size){
        if(0 == c->size){
            c->size = APHA_STACK_INIT_SIZE;
        }
        while(c->size - c->top < size){
            c->size += c->size >> 1;
        }
        c->stack = (char*) realloc (c->stack, c->size);
    }
    addr = c->stack + c->top;
    c->top += size;
    return addr;
}

static void* apha_pop(apha_context* c, size_t size){
    assert(c->top >= size);

    return c->stack + (c->top -= size);
}

static void apha_ignore_whitespace(apha_context* c){
    const char* p = c->json;
    while( *p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'){
        p++;
    }
    c->json = p;
}

static int apha_parse_literal(apha_value* v, apha_context* c, const char* literal, apha_type type){
    assert(*(c->json) == literal[0]);
    size_t i;
    for(i = 1; literal[i] != '\0'; i++){
        if(c->json[i] != literal[i]){
            return APHA_PARSE_INVALID_VALUE;
        }
    }
    c->json += i;
    v->type = type;
    return APHA_PARSE_OK;
}
#if 0
static int apha_parse_null(apha_value* v, apha_context* c){
    assert(*(c->json) == 'n');

    if( (c->json[1] != 'u') || (c->json[2] != 'l') || (c->json[3] != 'l') ){
        return APHA_PARSE_INVALID_VALUE;
    }
    c->json += 4;
    v->type = APHA_NULL;
    return APHA_PARSE_OK;
}

static int apha_parse_true(apha_value* v, apha_context* c){
    assert(c->json[0] == 't');

    if( (c->json[1] != 'r') || (c->json[2] != 'u') || (c->json[3] != 'e') ){
        return APHA_PARSE_INVALID_VALUE;
    }
    c->json += 4;
    v->type = APHA_TRUE;
    return APHA_PARSE_OK;
}

static int apha_parse_false(apha_value* v, apha_context* c){
    assert(*(c->json) == 'f');

    if( (c->json[1] != 'a') || (c->json[2] != 'l') || (c->json[3] != 's') || (c->json[4] != 'e')){
        return APHA_PARSE_INVALID_VALUE;
    }
    c->json += 5;
    v->type = APHA_FALSE;
    return APHA_PARSE_OK;
}
#endif

static int apha_parse_number(apha_value* v, apha_context* c){
    const char* p = c->json;
    if(*p == '-') { p++; }
    if(*p == '0') { p++; }
    else {
        if(!isDigit1To9(*p)) return APHA_PARSE_INVALID_VALUE;
        p++;
        while(isDigit(*p)) { p++; }
    }
    if(*p == '.') { 
        p++;
        if(!isDigit(*p)) return APHA_PARSE_INVALID_VALUE;
        p++;
        while(isDigit(*p)) { p++; }
    }
    if(*p == 'e' || *p == 'E') { 
        p++;
        if(*p == '+' || *p == '-') { p++; }
        while(isDigit(*p)) { p++; }
    }

    v->u.number = strtod(c->json, NULL);
    if(ERANGE == errno && (v->u.number == HUGE_VAL || v->u.number == -HUGE_VAL)){
        return APHA_PARSE_NUMBER_TOO_BIG;
    }
    c->json = p;
    v->type = APHA_NUMBER;
    return APHA_PARSE_OK;
}

static int apha_parse_string(apha_value* v, apha_context* c){
    size_t head = c->top, len;
    const char* p = c->json;
    assert(*p == '\"');
    p++;
    while(1){
        char ch = *p++;
        switch (ch) {
            case '\"' : 
                len = c->top - head;
                apha_set_string(v, (const char*) apha_pop(c, len), len);
                c->json = p;
                return APHA_PARSE_OK;
            case '\\' : // escape_part
                switch (*p++) { 
                    case '\"' :  PUTC(c, '\"'); break;
                    case '\\' :  PUTC(c, '\\'); break;
                    case '/'  :  PUTC(c, '/' ); break;
                    case 'b'  :  PUTC(c, '\b'); break;
                    case 'f'  :  PUTC(c, '\f'); break;
                    case 'n'  :  PUTC(c, '\n'); break;
                    case 'r'  :  PUTC(c, '\r'); break;
                    case 't'  :  PUTC(c, '\t'); break;
                    default :
                        c->top = head;
                        return APHA_PARSE_INVALID_STRING_ESCAPE;
                }
                break;
            case '\0':
                c->top = head;
                return APHA_PARSE_MISS_QUOTATION_MARK;
            default: // unescape_part 
                if((unsigned char) ch < 0x20){
                    c->top = head;
                    return APHA_PARSE_INVALID_STRING_CHAR;
                }
                PUTC(c, ch);
        }
    }
}
static int apha_parse_value(apha_value* v, apha_context* c){
    const char* p = c->json;
    
    switch(*p){
        case 'n' : return apha_parse_literal(v, c, "null",  APHA_NULL);
        case 't' : return apha_parse_literal(v, c, "true",  APHA_TRUE);
        case 'f' : return apha_parse_literal(v, c, "false", APHA_FALSE);
        case '\0' : return APHA_PARSE_EXCEPT_VALUE;
        case '\"' : return apha_parse_string(v, c);
        default : return apha_parse_number(v, c);
    }
}

int apha_parse(apha_value* v, const char* json){
    apha_context c;
    assert(v != NULL);
    c.json = json;
    c.stack = NULL;
    c.top = c.size = 0;
    v->type = APHA_NULL;
    apha_ignore_whitespace(&c);

    int ret = apha_parse_value(v, &c);
    if(ret == APHA_PARSE_OK){
        apha_ignore_whitespace(&c);
        if(*c.json != '\0'){
            v->type = APHA_NULL;
            ret = APHA_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

apha_type apha_get_type(apha_value* v){
    return v->type;
}

void apha_free(apha_value* v){
    assert(v != NULL);
    if(v->type == APHA_STRING){
        free(v->u.s.str);
    }
    v->type = APHA_NULL;
}

double apha_get_number(apha_value* v){
    return v->u.number;
}

const char* apha_get_string(const apha_value* v) {
    assert(v != NULL && v->type == APHA_STRING);
    return v->u.s.str;
}

void apha_set_string(apha_value* v, const char* str, size_t len){
    assert(v != NULL && (str != NULL || len == 0));
    apha_free(v);
    v->u.s.str = (char*) malloc ((len + 1) * sizeof(char));
    memcpy(v->u.s.str, str, len);
    v->u.s.str[len] = '\0';
    v->u.s.len = len;
    v->type = APHA_STRING;
}

size_t apha_get_string_length(const apha_value* v) {
    assert(v != NULL && v->type == APHA_STRING);
    return v->u.s.len;
}
