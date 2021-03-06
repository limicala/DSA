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

static const char* lept_parse_hex4(const char* p, unsigned* u){
    int i;
    *u = 0;
    for(i = 0; i < 4; i++){
        char ch = *p++;
        *u <<= 4;
        
        if('0' <= ch && ch <= '9') { *u |= ch - '0'; }
        else if('a' <= ch && ch <= 'f') { *u |= ch - 'a' + 10; }
        else if('A' <= ch && ch <= 'F') { *u |= ch - 'A' + 10; }
        else return NULL;
    }
    return p;
}

static void lept_encode_utf8(apha_context* c, unsigned u){
    if (u <= 0x007F){
        PUTC(c, u);
    }else if (u <= 0x07FF) {
        PUTC(c, 0xC0 | u >> 6);
        PUTC(c, 0x80 | u);
    }else if (u <= 0xFFFF) {
        PUTC(c, 0xE0 | u >> 12);
        PUTC(c, 0x80 | u >> 6);
        PUTC(c, 0x80 | u);
    }else {
        PUTC(c, 0xF0 | u >> 18);
        PUTC(c, 0x80 | u >> 12);
        PUTC(c, 0x80 | u >> 6);
        PUTC(c, 0x80 | u);
    }
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
    unsigned u; // 4 bytes
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
                    case 'u'  :
                        if (!(p = lept_parse_hex4(p, &u))){
                            c->top = head;
                            return APHA_PARSE_INVALID_UNICODE_HEX;
                        }
                        //printf("u[%u]\n", u);
                        if(0xD800 <= u && u <= 0xDBFF){
                            if(*p++ != '\\') return APHA_PARSE_INVALID_UNICODE_SURROGATE;
                            if(*p++ != 'u')  return APHA_PARSE_INVALID_UNICODE_SURROGATE;
                            unsigned u2;
                            if (!(p = lept_parse_hex4(p, &u2))){
                                c->top = head;
                                return APHA_PARSE_INVALID_UNICODE_HEX;
                            }
                            if(!(0xDC00 <= u2 && u2 <= 0xDFFF)){
                                c->top = head;
                                return APHA_PARSE_INVALID_UNICODE_SURROGATE;
                            }
                            u = 0x10000 + (u - 0xD800) * 0x400 + (u2 - 0xDC00);
                        }
                        lept_encode_utf8(c, u);
                        break;
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
static int apha_parse_value(apha_value* v, apha_context* c);

static int apha_parse_array(apha_value* v, apha_context* c){
    size_t size = 0;
    int ret;
    assert(*(c->json) == '[');

    c->json += 1;

    apha_ignore_whitespace(c);
    if(*(c->json) == ']'){
        c->json += 1;
        v->type = APHA_ARRAY;
        v->u.a.e = NULL;
        v->u.a.size = 0;
        return APHA_PARSE_OK;
    }

    while(1){
        apha_value e;
        apha_init(&e);

        if((ret = apha_parse_value(&e, c)) != APHA_PARSE_OK){
            break;
        }
        memcpy(apha_push(c, sizeof(apha_value)), &e, sizeof(apha_value));
        size += 1;
        apha_ignore_whitespace(c);
        if(*(c->json) == ','){
            c->json += 1;
            apha_ignore_whitespace(c);
        }else if(*(c->json) == ']'){
            c->json += 1;
            v->u.a.size = size;
            v->type = APHA_ARRAY;

            v->u.a.e = (apha_value*) malloc (sizeof(apha_value) * size);
            memcpy(v->u.a.e, apha_pop(c, sizeof(apha_value) * size), sizeof(apha_value) * size);
            return APHA_PARSE_OK;
        }else{
            // something wrong
            ret = APHA_PARSE_INVALID_VALUE;
            break;
        }
    }

    for(int i = 0; i < size; i++){
        apha_free(apha_pop(c, sizeof(apha_value)));
    }
    return ret;
}

static int apha_parse_value(apha_value* v, apha_context* c){
    const char* p = c->json;
    
    switch(*p){
        case 'n' : return apha_parse_literal(v, c, "null",  APHA_NULL);
        case 't' : return apha_parse_literal(v, c, "true",  APHA_TRUE);
        case 'f' : return apha_parse_literal(v, c, "false", APHA_FALSE);
        case '\0' : return APHA_PARSE_EXCEPT_VALUE;
        case '\"' : return apha_parse_string(v, c);
        case '[' : return apha_parse_array(v, c);
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

size_t apha_get_array_size(const apha_value* v) {
    assert(v != NULL && v->type == APHA_ARRAY);
    return v->u.a.size;
}

apha_value* apha_get_array_element(const apha_value* v, size_t index){
    return &(v->u.a.e[index]);
}

size_t apha_get_object_size(const apha_value* v){
    return v->u.o.size;
}
const char* apha_get_object_key(const apha_value* v, int index){
    return (v->u.o.m[index]).k;
}
size_t apha_get_object_key_length(const apha_value* v, int index){
    return 0;
}
apha_value* apha_get_object_value(const apha_value* v, int index){
    return NULL;
}
