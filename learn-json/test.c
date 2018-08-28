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

#define TEST_NUMBER(except, json) do{\
    apha_value v;\
    ASSERT_INT(APHA_PARSE_OK, apha_parse(&v, json));\
    ASSERT_INT(APHA_NUMBER, apha_get_type(&v));\
    ASSERT_DOUBLE(except, apha_get_number(&v));\
}while(0)

static void test_parse_number(){
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000"); /* must underflow */

    TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
    TEST_NUMBER( 4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
    TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    TEST_NUMBER( 2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
    TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_NUMBER( 2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
    TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_NUMBER( 1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
    TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
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

    /* invalid number */
    TEST_ERROR(APHA_PARSE_ROOT_NOT_SINGULAR, "0123"); /* after zero should be '.' or nothing */
    TEST_ERROR(APHA_PARSE_ROOT_NOT_SINGULAR, "0x0");
    TEST_ERROR(APHA_PARSE_ROOT_NOT_SINGULAR, "0x123");
}

static void test_parse_invalid_value(){
#if 1
    TEST_ERROR(APHA_PARSE_INVALID_VALUE, " fz");
    TEST_ERROR(APHA_PARSE_INVALID_VALUE, " nula");
#endif
    /* invalid number */
    TEST_ERROR(APHA_PARSE_INVALID_VALUE, "+0");
    TEST_ERROR(APHA_PARSE_INVALID_VALUE, "+1");
    TEST_ERROR(APHA_PARSE_INVALID_VALUE, ".123"); /* at least one digit before '.' */
    TEST_ERROR(APHA_PARSE_INVALID_VALUE, "1.");   /* at least one digit after '.' */
    TEST_ERROR(APHA_PARSE_INVALID_VALUE, "nan");
}

static void test_parse(){
#if 0
    test_parse_null();
    test_parse_true();
    test_parse_false();
#endif  
    test_parse_number_too_big();
    test_parse_not_singular();
    test_parse_invalid_value();
    test_parse_number();
#if 0
    test_parse_invalid_value();
#endif
}
#include <errno.h>
int main(){
    test_parse();
    printf("%d/%d passed\n", test_pass, test_count);
    return 1;
}