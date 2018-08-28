#include "aphajson.h"
#include "env.h"

typedef struct {
    const char* json;
}apha_context;

#define isDigit1To9(ch) ('1' <= ch && ch <= '9')
#define isDigit(ch)     ('0' <= ch && ch <= '9')

static void apha_ignore_whitespace(apha_context* c){
    const char* p = c->json;
    while( *p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'){
        p++;
    }
    c->json = p;
}

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

    v->number = strtod(c->json, NULL);
    if(ERANGE == errno && (v->number == HUGE_VAL || v->number == -HUGE_VAL)){
        return APHA_PARSE_NUMBER_TOO_BIG;
    }
    c->json = p;
    v->type = APHA_NUMBER;
    return APHA_PARSE_OK;
}

static int apha_parse_value(apha_value* v, apha_context* c){
    const char* p = c->json;
    
    switch(*p){
        case 'n' : return apha_parse_null(v, c);
        case 't' : return apha_parse_true(v, c);
        case 'f' : return apha_parse_false(v, c);
        case '\0' : return APHA_PARSE_EXCEPT_VALUE;
        default : return apha_parse_number(v, c);
    }
}

int apha_parse(apha_value* v, const char* json){
    apha_context c;
    assert(v != NULL);
    c.json = json;
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

double apha_get_number(apha_value* v){
    return v->number;
}