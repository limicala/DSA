#include "aphajson.h"
#include "env.h"

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
    assert(*(c->json) == 't');

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

static int apha_parse_value(apha_value* v, apha_context* c){
    const char* p = c->json;
    
    switch(*p){
        case 'n' : return apha_parse_null(v, c);
        case 't' : return apha_parse_true(v, c);
        case 'f' : return apha_parse_false(v, c);
        case '\0' : return APHA_PARSE_EXCEPT_VALUE;
        default : return APHA_PARSE_INVALID_VALUE;
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
        if(*(c.json) == '\0'){
            return APHA_PARSE_OK;
        }else{
            return APHA_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

apha_type apha_get_type(apha_value* v){
    return v->type;
}