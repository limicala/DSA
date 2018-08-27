#include "aphajson.h"
#include "env.h"

int test_pass = 0;
int test_count = 0;

#define ASSERT_BASE(equality, except, actual, format) do {\
    test_count++;\
    if(equality) {\
        test_pass++;\
    }else {\
        fprintf(stderr, "%s:%d except: " format " actual: " format "\n", \
            __FILE__, __LINE__, except, actual);\
    }\
}while(0)

#define ASSERT_INT(except, actual) ASSERT_BASE( (except) == (actual), except, actual, "%d") 
#define ASSERT_DOUBLE(except, actual) ASSERT_BASE( (except) == (actual), except, actual, "%lf")

#define TEST_ERROR(expect_ret, json) do {\
    apha_value v; \
    ASSERT_INT(expect_ret, apha_parse(&v, json));\
    ASSERT_INT(APHA_NULL, apha_get_type(&v));\
} while(0)

static void test_parse_null(){
    apha_value v;
    
    ASSERT_INT(APHA_PARSE_OK, apha_parse(&v, "null"));
    ASSERT_INT(APHA_NULL, apha_get_type(&v));
}

static void test_parse_true(){
    apha_value v;

    ASSERT_INT(APHA_PARSE_OK, apha_parse(&v, "  true "));
    ASSERT_INT(APHA_TRUE, apha_get_type(&v));
}

static void test_parse_false(){
    apha_value v;

    ASSERT_INT(APHA_PARSE_OK, apha_parse(&v, "false "));
    ASSERT_INT(APHA_FALSE, apha_get_type(&v));
}

static void test_parse_number(){
    apha_value v;

    ASSERT_INT(APHA_PARSE_OK, apha_parse(&v, "1.4"));
    ASSERT_INT(APHA_NUMBER, apha_get_type(&v));
    ASSERT_DOUBLE(1.4, apha_get_number(&v));
}

static void test_parse_number_too_big() {
#if 1
    TEST_ERROR(APHA_PARSE_NUMBER_TOO_BIG, "1e1009");
    TEST_ERROR(APHA_PARSE_NUMBER_TOO_BIG, "-1e309");
#endif
}

static void test_parse_not_singular(){
    TEST_ERROR(APHA_PARSE_ROOT_NOT_SINGULAR, "null x");
    TEST_ERROR(APHA_PARSE_ROOT_NOT_SINGULAR, "null null");
}

static void test_parse_invalid_value(){
    TEST_ERROR(APHA_PARSE_INVALID_VALUE, " fz");
    TEST_ERROR(APHA_PARSE_INVALID_VALUE, " nula");
}

static void test_parse(){
#if 0
    test_parse_null();
    test_parse_true();
    test_parse_false();

    test_parse_number();
#endif    
    test_parse_number_too_big();
#if 0
    test_parse_not_singular();
    test_parse_invalid_value();
#endif
}

int main(){
    test_parse();
    printf("%d/%d passed\n", test_pass, test_count);
    return 1;
}