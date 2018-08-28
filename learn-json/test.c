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
#define ASSERT_STR(except, actual, actual_len) \
    ASSERT_BASE(sizeof(except) - 1 == actual_len && memcmp(except, actual, actual_len) == 0, except, actual, "%s")

#define ASSERT_TRUE(actual)  ASSERT_BASE( actual != 0, "true", "false", "%s")
#define ASSERT_FALSE(actual) ASSERT_BASE( actual == 0, "false", "true", "%s")

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

#define TEST_STRING(except, json) do {\
    apha_value v;\
    apha_init(&v);\
    ASSERT_INT(APHA_PARSE_OK, apha_parse(&v, json));\
    ASSERT_INT(APHA_STRING, apha_get_type(&v));\
    ASSERT_STR(except, apha_get_string(&v), apha_get_string_length(&v));\
    apha_free(&v);\
} while(0)

static void test_parse_string() {
    TEST_STRING("", "\"\"");
    TEST_STRING("Hello", "\"Hello\"");
    TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
    TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
}

static void test_parse_missing_quotation_mark() {
    TEST_ERROR(APHA_PARSE_MISS_QUOTATION_MARK, "\"");
    TEST_ERROR(APHA_PARSE_MISS_QUOTATION_MARK, "\"abc");
}

static void test_parse_invalid_string_escape() {
    TEST_ERROR(APHA_PARSE_INVALID_STRING_ESCAPE, "\"\\v\"");
    TEST_ERROR(APHA_PARSE_INVALID_STRING_ESCAPE, "\"\\'\"");
    TEST_ERROR(APHA_PARSE_INVALID_STRING_ESCAPE, "\"\\0\"");
    TEST_ERROR(APHA_PARSE_INVALID_STRING_ESCAPE, "\"\\x12\"");
}

static void test_parse_invalid_string_char() {
    TEST_ERROR(APHA_PARSE_INVALID_STRING_CHAR, "\"\x01\"");
    TEST_ERROR(APHA_PARSE_INVALID_STRING_CHAR, "\"\x1F\"");
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

static void test_parse_invalid_unicode_hex() {
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_HEX, "\"\\u\"");
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_HEX, "\"\\u0\"");
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_HEX, "\"\\u01\"");
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_HEX, "\"\\u012\"");
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_HEX, "\"\\u/000\"");
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_HEX, "\"\\uG000\"");
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_HEX, "\"\\u0/00\"");
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_HEX, "\"\\u0G00\"");
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_HEX, "\"\\u0/00\"");
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_HEX, "\"\\u00G0\"");
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_HEX, "\"\\u000/\"");
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_HEX, "\"\\u000G\"");
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_HEX, "\"\\u 123\"");
}

static void test_parse_invalid_unicode_surrogate() {
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\"");
#if 0
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uDBFF\"");
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\\\\"");
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uDBFF\"");
    TEST_ERROR(APHA_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uE000\"");
#endif
}

static void test_parse(){
#if 0
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_number_too_big();
    test_parse_not_singular();
    test_parse_invalid_value();
    test_parse_number();
    test_parse_invalid_value();
#endif
#if 0
    test_parse_string();
    test_parse_missing_quotation_mark();
    test_parse_invalid_string_escape();
    test_parse_invalid_string_char();
    test_parse_invalid_unicode_hex();
#endif
    
    test_parse_invalid_unicode_surrogate();
}
int main(){
    test_parse();
    printf("%d/%d passed\n", test_pass, test_count);
    return 1;
}