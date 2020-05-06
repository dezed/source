#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include "csal/csal.h"

#define TC_TEST( tf, expr ) test_case_test( tf, expr, #expr, __FILE__, __LINE__ )

#define TC_TEST_EQUAL_INT( tc, val, expected ) test_case_equal_int( tc, val, expected, __FILE__, __LINE__ )
#define TC_TEST_EQUAL_FLOAT( tc, val, expected ) test_case_equal_float( tc, val, expected, __FILE__, __LINE__ )
#define TC_TEST_EQUAL_DOUBLE( tc, val, expected ) test_case_equal_double( tc, val, expected, __FILE__, __LINE__ )

#define TS_TEST_CASE( tf, f ) test_suite_add_test_case( tf, #f, f )

struct _test_suite_t;
struct _test_case_t;

typedef int (*TEST_CASE_FUNCTION)( struct _test_case_t* );

enum TEST_SUITE_CTRL_FLAGS
{
    TEST_SUITE_CTRL_NONE=0
        , TEST_SUITE_CTRL_EXIT_ON_FAIL=1
        , TEST_SUITE_CTRL_VERBOSE=2
};


typedef struct _test_case_t
{
    TEST_CASE_FUNCTION f;
    char szname[ 256];
    int npassed;
    int nfailed;

    int ctrl_flags;
    struct _test_suite_t* test_suite;

} test_case_t;

typedef struct _test_suite_t
{
    unsigned int ctrl_flags;
    int ntestcases;
    test_case_t* testcases;

} test_suite_t;



int test_case_init( test_case_t* self, test_suite_t* owner )
{
    int err = 0;
    
    memset( self, 0, sizeof( test_case_t ) );

    self->test_suite = owner;

    return err;
}

int test_case_name_set( test_case_t* self, const char* pszname )
{
    int err =0;

    strncpy( self->szname, pszname, 255 );

    return err;
}

char* test_case_name_get( test_case_t* self )
{
    return self->szname;
}

int test_case_finish( test_case_t* self )
{
    int err = 0;

    return err;
}

int test_suite_add_test_case( test_suite_t* self, const char* pszname, TEST_CASE_FUNCTION f)
{
    int err = 0;

    test_case_t*  tc = NULL;


    self->ntestcases++;
    self->testcases = (test_case_t*)realloc( self->testcases, sizeof(test_case_t) * self->ntestcases );

    tc = self->testcases + self->ntestcases-1;

    test_case_init( tc, self );
    test_case_name_set( tc, pszname );
    tc->f = f;


    printf( "\nTest case : %s : START\n", pszname );

    tc->f( tc );
    
    printf( "Test Case : %s : %d/%d passed\n",pszname,  tc->npassed, tc->npassed + tc->nfailed );
    printf( "Test Case : %s : %d/%d failed\n", pszname, tc->nfailed, tc->npassed + tc->nfailed );
    printf( "Test case : %s : END\n", pszname );

    return err;
}

int test_suite_init( test_suite_t* self, unsigned int ctrl_flags )
{
    self->ctrl_flags = ctrl_flags;

    self->ntestcases = 0;
    self->testcases = NULL;
    return 0;
}

static int _test_case_passed( test_case_t* self, int res, const char* expr, const char* pszfile, unsigned int lineno )
{
    int err = 0;

    self->npassed++;

    printf( "%s(%d) : %s : %s\n", pszfile, lineno, expr, "PASS" );

    return err;
}

static int _test_case_failed( test_case_t* self, int res, const char* expr, const char* pszfile, unsigned int lineno )
{
    int err = 0;

    self->nfailed++;

    printf( "%s(%d) : %s : %s\n", pszfile, lineno, expr, "FAIL" );

    if( !res && self->ctrl_flags & TEST_SUITE_CTRL_EXIT_ON_FAIL )
        exit(1);

    return err;
}



int test_case_test( test_case_t* self, int res, const char* expr, const char* file, unsigned int lineno )
{
    if( res )
        _test_case_passed( self, res, expr, file, lineno );
    else
        _test_case_failed( self, res, expr, file, lineno );

    return 0;
}

int test_case_equal_int( test_case_t* self, int val, int expected, const char* pszfile, unsigned int lineno )
{
    int res = (val == expected);

    char buffer[ 512];

    sprintf( buffer, "%d == %d", val, expected );

    if( res )
        _test_case_passed( self, res, buffer, pszfile, lineno );
    else
        _test_case_failed( self, res, buffer, pszfile, lineno );

    return 0;
}

int test_case_equal_float( test_case_t* self, float val, float expected, const char* pszfile, unsigned int lineno )
{
    int res = (val == expected);

    char buffer[ 512];

    sprintf( buffer, "%f == %f", val, expected );

    if( res )
        _test_case_passed( self, res, buffer, pszfile, lineno );
    else
        _test_case_failed( self, res, buffer, pszfile, lineno );

    return 0;
}

int test_case_equal_double( test_case_t* self, double val, double expected, const char* pszfile, unsigned int lineno )
{
    int res = (val == expected);

    char buffer[ 512];

    sprintf( buffer, "%f == %f", val, expected );

    if( res )
        _test_case_passed( self, res, buffer, pszfile, lineno );
    else
        _test_case_failed( self, res, buffer, pszfile, lineno );

    return 0;
}


int test_suite_finish( test_suite_t* self )
{
    size_t i = 0;


    int npassed=0;
    int nfailed=0;

    for( i = 0; i < self->ntestcases; ++i )
    {
        test_case_t* tc = self->testcases + i;
    
        npassed += tc->npassed;
        nfailed += tc->nfailed;

        test_case_finish( tc );
    }

    printf( "\n" );
    printf( "%d/%d tests failed\n", nfailed, nfailed+npassed );
    printf( "%d/%d tests passed\n", npassed, nfailed+npassed );
    printf( "%d test cases\n", self->ntestcases );

    free( self->testcases );

    return 0;
}


int float_cmp( float val1, float val2, float tol )
{
    return val1-val2 < tol;
}

#define FLOAT_COMPARE(x,y) float_cmp(x,y,0.001)

int test_attrib_cast( test_case_t* tf )
{
    csal_attrib_t* csal_attrib_ptr = NULL;
    csal_attrib_uint8_t* at_uint8 = csal_attrib_uint8_create(8);
    csal_attrib_uint16_t* at_uint16 = csal_attrib_uint16_create(16);
    csal_attrib_uint32_t* at_uint32 = csal_attrib_uint32_create(32);
    csal_attrib_uint64_t* at_uint64 = csal_attrib_uint64_create(64);
    csal_attrib_int8_t* at_int8 = csal_attrib_int8_create(8);
    csal_attrib_int16_t* at_int16 = csal_attrib_int16_create(16);
    csal_attrib_int32_t* at_int32 = csal_attrib_int32_create(32);
    csal_attrib_int64_t* at_int64 = csal_attrib_int64_create(64);
    csal_attrib_float32_t* at_float32 = csal_attrib_float32_create(1.1);
    csal_attrib_float64_t* at_float64 = csal_attrib_float64_create(2.2);
    csal_attrib_string_t* at_string = csal_attrib_string_create( "a string");


    csal_attrib_uint8_cast( at_uint8, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr ));
    TC_TEST( tf, NULL != csal_attrib_ptr );

    csal_attrib_uint16_cast( at_uint16, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr ));
    TC_TEST( tf, NULL != csal_attrib_ptr );

    csal_attrib_uint32_cast( at_uint32, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr ));
    TC_TEST( tf, NULL != csal_attrib_ptr );

    csal_attrib_uint64_cast( at_uint64, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr ));
    TC_TEST( tf, NULL != csal_attrib_ptr );


    csal_attrib_int8_cast( at_int8, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr ));
    TC_TEST( tf, NULL != csal_attrib_ptr );

    csal_attrib_int16_cast( at_int16, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr ));
    TC_TEST( tf, NULL != csal_attrib_ptr );

    csal_attrib_int32_cast( at_int32, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr ));
    TC_TEST( tf, NULL != csal_attrib_ptr );

    csal_attrib_int64_cast( at_int64, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr ));
    TC_TEST( tf, NULL != csal_attrib_ptr );


    csal_attrib_float32_cast( at_float32, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr ));
    TC_TEST( tf, NULL != csal_attrib_ptr );

    csal_attrib_float64_cast( at_float64, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr ));
    TC_TEST( tf, NULL != csal_attrib_ptr );

    csal_attrib_string_cast( at_string, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr ));
    TC_TEST( tf, NULL != csal_attrib_ptr );




    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_uint8 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_uint16 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_uint32 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_uint64 ) );


    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_int8 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_int16 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_int32 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_int64 ) );

    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_float32 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_float64 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_string ) );

    return 0;
}

int test_constants( test_case_t* tf )
{
#if 0
    TC_TEST( tf, 0==strcmp( csal_attr_int8_str, "int8" ) );
    TC_TEST( tf, 0==strcmp( csal_attr_int16_str, "int16" ) );
    TC_TEST( tf, 0==strcmp( csal_attr_int32_str, "int32" ) );
    TC_TEST( tf, 0==strcmp( csal_attr_int64_str, "int64" ) );

    TC_TEST( tf, 0==strcmp( csal_attr_uint8_str, "uint8" ) );
    TC_TEST( tf, 0==strcmp( csal_attr_uint16_str, "uint16" ) );
    TC_TEST( tf, 0==strcmp( csal_attr_uint32_str, "uint32" ) );
    TC_TEST( tf, 0==strcmp( csal_attr_uint64_str, "uint64" ) );

    TC_TEST( tf, 0==strcmp( csal_attr_float32_str, "float32" ) );
    TC_TEST( tf, 0==strcmp( csal_attr_float64_str, "float64" ) );
    TC_TEST( tf, 0==strcmp( csal_attr_string_str, "string" ) );
    TC_TEST( tf, 0==strcmp( csal_attr_branch_str, "branch" ) );
#endif
    return 0;
}

int test_construction( test_case_t* tf )
{
    csal_attrib_uint8_t* at_uint8 = csal_attrib_uint8_create(8);
    csal_attrib_uint16_t* at_uint16 = csal_attrib_uint16_create(16);
    csal_attrib_uint32_t* at_uint32 = csal_attrib_uint32_create(32);
    csal_attrib_uint64_t* at_uint64 = csal_attrib_uint64_create(64);

    csal_attrib_int8_t* at_int8 = csal_attrib_int8_create(8);
    csal_attrib_int16_t* at_int16 = csal_attrib_int16_create(16);
    csal_attrib_int32_t* at_int32 = csal_attrib_int32_create(32);
    csal_attrib_int64_t* at_int64 = csal_attrib_int64_create(64);
    csal_attrib_float32_t* at_float = csal_attrib_float32_create(1.1);
    csal_attrib_float64_t* at_double = csal_attrib_float64_create(2.2);
    csal_attrib_string_t* at_string = csal_attrib_string_create( "a string");





#if 0
    csal_attrib_branch_t* at_branch = csal_attrib_branch_create();
#endif
    CSAL_ATTRIBUTE_TYPE at_int8_type = csal_attrib_type( (csal_attrib_t*)at_int8 );

    TC_TEST( tf, CSAL_ATTR_UINT8 == csal_attrib_type( (csal_attrib_t*)at_uint8) );
    TC_TEST( tf, CSAL_ATTR_UINT16 == csal_attrib_type( (csal_attrib_t*)at_uint16 ) );
    TC_TEST( tf, CSAL_ATTR_UINT32 == csal_attrib_type( (csal_attrib_t*)at_uint32 ) );
    TC_TEST( tf, CSAL_ATTR_UINT64 == csal_attrib_type( (csal_attrib_t*)at_uint64 ) );
    TC_TEST( tf, CSAL_ATTR_FLOAT32 == csal_attrib_type( (csal_attrib_t*)at_float ) );
    TC_TEST( tf, CSAL_ATTR_FLOAT64 == csal_attrib_type( (csal_attrib_t*)at_double ) );

    TC_TEST( tf, CSAL_ATTR_INT8 == at_int8_type );
    TC_TEST( tf, CSAL_ATTR_INT16 == csal_attrib_type( (csal_attrib_t*)at_int16 ) );
    TC_TEST( tf, CSAL_ATTR_INT32 == csal_attrib_type( (csal_attrib_t*)at_int32 ) );
    TC_TEST( tf, CSAL_ATTR_INT64 == csal_attrib_type( (csal_attrib_t*)at_int64 ) );
    TC_TEST( tf, CSAL_ATTR_STRING == csal_attrib_type( (csal_attrib_t*)at_string ) );
#if 0
    TC_TEST( tf, CSAL_ATTR_BRANCH == csal_attrib_type( (csal_attrib_t*)at_branch ) );
#endif
    TC_TEST( tf, 8 == csal_attrib_uint8_value_get( at_uint8 ) );
    TC_TEST( tf, 16 == csal_attrib_uint16_value_get( at_uint16 ) );
    TC_TEST( tf, 32 == csal_attrib_uint32_value_get( at_uint32 ) );
    TC_TEST( tf, 64 == csal_attrib_uint64_value_get( at_uint64 ) );

    TC_TEST( tf, FLOAT_COMPARE( 1.1, csal_attrib_float32_value_get( at_float ) ) );
    TC_TEST( tf, FLOAT_COMPARE( 1.1, csal_attrib_float64_value_get( at_double ) ) );


    TC_TEST( tf, 8 == csal_attrib_int8_value_get( at_int8 ) );
    TC_TEST( tf, 16 == csal_attrib_int16_value_get( at_int16 ) );
    TC_TEST( tf, 32 == csal_attrib_int32_value_get( at_int32 ) );
    TC_TEST( tf, 64 == csal_attrib_int64_value_get( at_int64 ) );

    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_int8_value_set( at_int8, 1 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_int16_value_set( at_int16, 2 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_int32_value_set( at_int32, 3 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_int64_value_set( at_int64, 4 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_float32_value_set( at_float, -1.1 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_float64_value_set( at_double, -2.2 ) );

    TC_TEST( tf, 1 == csal_attrib_int8_value_get( at_int8 ) );
    TC_TEST( tf, 2 == csal_attrib_int16_value_get( at_int16 ) );
    TC_TEST( tf, 3 == csal_attrib_int32_value_get( at_int32 ) );
    TC_TEST( tf, 4 == csal_attrib_int64_value_get( at_int64 ) );
    TC_TEST( tf, FLOAT_COMPARE( -1.1, csal_attrib_float32_value_get( at_float ) ) );
    TC_TEST( tf, FLOAT_COMPARE( -2.2, csal_attrib_float64_value_get( at_double ) ) );



    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_uint8_value_set( at_uint8, 1 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_uint16_value_set( at_uint16, 2 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_uint32_value_set( at_uint32, 3 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_uint64_value_set( at_uint64, 4 ) );

    TC_TEST( tf, 1 == csal_attrib_uint8_value_get( at_uint8 ) );
    TC_TEST( tf, 2 == csal_attrib_uint16_value_get( at_uint16 ) );
    TC_TEST( tf, 3 == csal_attrib_uint32_value_get( at_uint32 ) );
    TC_TEST( tf, 4 == csal_attrib_uint64_value_get( at_uint64 ) );



    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_uint8 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_uint16 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_uint32 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_uint64 ) );


    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_int8 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_int16 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_int32 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_int64 ) );

    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_string ) );
#if 0
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_branch ) );
#endif
    return 0;
}
#if 0
int test_branch( test_suite_t* tf )
{
    csal_attrib_int8_t* at_int8 = csal_attrib_int8_create(8);
    csal_attrib_int16_t* at_int16 = csal_attrib_int16_create(16);
    csal_attrib_int32_t* at_int32 = csal_attrib_int32_create(32);
    csal_attrib_int64_t* at_int64 = csal_attrib_int64_create(64);
    csal_attrib_string_t* at_string = csal_attrib_string_create( "a string");


    csal_attrib_branch_t* at_branch = csal_attrib_branch_create();


    csal_attrib_branch_set( at_branch, "key1", (csal_attrib_t*)at_int8 );
    csal_attrib_branch_set( at_branch, "key2", (csal_attrib_t*)at_int16 );
    csal_attrib_branch_set( at_branch, "key32", (csal_attrib_t*)at_int32 );


    TC_TEST( tf, csal_true == csal_attrib_branch_has( at_branch, "key1" ) );
    TC_TEST( tf, csal_false == csal_attrib_branch_has( at_branch, "nokey" ) );

    csal_attrib_t* at_got = csal_attrib_branch_get( at_branch, "key1" );
    TC_TEST( tf, NULL != at_got );

    TC_TEST( tf, CSAL_ATTR_INT8 == csal_attrib_type( at_got ) ) ;
    TC_TEST( tf, 8==csal_attrib_int8_value_get( (csal_attrib_int8_t*)at_got ) );

    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_branch_remove( at_branch, "key1" ) );

    TC_TEST( tf, csal_false == csal_attrib_branch_has( at_branch, "key1" ) );

    TC_TEST( tf, "ds1" && CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_int8 ) );

    TC_TEST( tf, "ds2" && CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_int16 ) );

    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_int32 ) );

    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_int64 ) );
    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_string ) );


    TC_TEST( tf, CSAL_ERR_NONE == csal_attrib_destroy( (csal_attrib_t*)at_branch ) );
    return 0;
}
#endif
int test_bool_array( test_case_t* tf )
{

    csal_attrib_t* csal_attrib_ptr = NULL;
    csal_attrib_array_t* csal_array_ptr = NULL;

    int csal_err = 0;
    uint64_t dims[2];

    uint64_t shape[2];
    uint64_t nshape;

    uint64_t idxs[2];
    uint64_t ndims=2;

    int i,j;

    dims[0]=3;
    dims[1]=4;
    csal_attrib_array_bool_t* at_arr = csal_attrib_array_bool_create( dims, ndims );

    csal_attrib_array_bool_cast( at_arr, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr) );

    TC_TEST( tf, CSAL_ERR_NONE == csal_err );

    TC_TEST( tf, NULL != csal_attrib_ptr );




    TC_TEST( tf, NULL != at_arr );

    TC_TEST( tf, CSAL_ATTR_ARRAY == csal_attrib_type( (csal_attrib_t*)at_arr ) );

    TC_TEST( tf, csal_true == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_ARRAY ) );

    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_BOOL ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_STRING ) );

    csal_err = csal_attrib_array_shape( (csal_attrib_array_t*)(at_arr), shape, &nshape, 2 );


    TC_TEST(tf, CSAL_ERR_NONE == csal_err );
    TC_TEST(tf, 2 == nshape );



    csal_err = csal_attrib_array_shape( (csal_attrib_array_t*)(at_arr), shape, &nshape, 1 );

    TC_TEST(tf, 1 == csal_err );

    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            csal_attrib_array_bool_element_set( at_arr, idxs, ndims, i==j);
        }
    }

    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            TC_TEST( tf, (i==j) == csal_attrib_array_bool_element_get( at_arr, idxs, ndims) );
        }
    }

    csal_err = csal_attrib_array_bool_cast( at_arr, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr) );

    csal_err = csal_attrib_cast( csal_attrib_ptr, IID_CSAL_ATTRIB_ARRAY_BOOL, (void**)(&csal_array_ptr) );

    TC_TEST( tf, CSAL_ERR_NONE == csal_err );
    TC_TEST( tf, NULL != csal_array_ptr );

    csal_err = csal_attrib_cast( csal_attrib_ptr, IID_CSAL_ATTRIB_ARRAY_INT16, (void**)(&csal_array_ptr) );

    TC_TEST( tf, 1 == csal_err );
    TC_TEST( tf, NULL == csal_array_ptr );

    csal_err = csal_attrib_cast( csal_attrib_ptr, IID_CSAL_ATTRIB_ARRAY_INT32, (void**)(&csal_array_ptr) );

    TC_TEST( tf, 1 == csal_err );
    TC_TEST( tf, NULL == csal_array_ptr );

    csal_err = csal_attrib_cast( csal_attrib_ptr, IID_CSAL_ATTRIB_ARRAY_INT64, (void**)(&csal_array_ptr) );

    TC_TEST( tf, 1 == csal_err );
    TC_TEST( tf, NULL == csal_array_ptr );

    csal_err = csal_attrib_cast( csal_attrib_ptr, IID_CSAL_ATTRIB_ARRAY_UINT64, (void**)(&csal_array_ptr) );

    TC_TEST( tf, 1 == csal_err );
    TC_TEST( tf, NULL == csal_array_ptr );


    csal_attrib_destroy( (csal_attrib_t*)(at_arr) );

    return 0;
}

int test_int8_array( test_case_t* tf )
{

    csal_attrib_t* csal_attrib_ptr = NULL;
    csal_attrib_array_t* csal_array_ptr = NULL;

    int csal_err = 0;
    uint64_t dims[2];

    uint64_t shape[2];
    uint64_t nshape;

    uint64_t idxs[2];
    uint64_t ndims=2;

    int i,j;

    dims[0]=3;
    dims[1]=4;
    csal_attrib_array_int8_t* at_arr = csal_attrib_array_int8_create( dims, ndims );

    csal_attrib_array_int8_cast( at_arr, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr) );

    TC_TEST( tf, CSAL_ERR_NONE == csal_err );

    TC_TEST( tf, NULL != csal_attrib_ptr );




    TC_TEST( tf, NULL != at_arr );

    TC_TEST( tf, CSAL_ATTR_ARRAY == csal_attrib_type( (csal_attrib_t*)at_arr ) );

    TC_TEST( tf, csal_true == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_ARRAY ) );

    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_BOOL ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_STRING ) );

    csal_err = csal_attrib_array_shape( (csal_attrib_array_t*)(at_arr), shape, &nshape, 2 );


    TC_TEST(tf, CSAL_ERR_NONE == csal_err );
    TC_TEST(tf, 2 == nshape );



    csal_err = csal_attrib_array_shape( (csal_attrib_array_t*)(at_arr), shape, &nshape, 1 );

    TC_TEST(tf, 1 == csal_err );

    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            csal_attrib_array_int8_element_set( at_arr, idxs, ndims, i*j);
        }
    }

    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            TC_TEST( tf, i*j==csal_attrib_array_int8_element_get( at_arr, idxs, ndims) );
        }
    }



#if 1
    csal_err = csal_attrib_array_int8_cast( at_arr, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr) );

    csal_err = csal_attrib_cast( csal_attrib_ptr, IID_CSAL_ATTRIB_ARRAY_INT8, (void**)(&csal_array_ptr) );

    TC_TEST( tf, CSAL_ERR_NONE == csal_err );
    TC_TEST( tf, NULL != csal_array_ptr );

    csal_err = csal_attrib_cast( csal_attrib_ptr, IID_CSAL_ATTRIB_ARRAY_INT16, (void**)(&csal_array_ptr) );

    TC_TEST( tf, 1 == csal_err );
    TC_TEST( tf, NULL == csal_array_ptr );

    csal_err = csal_attrib_cast( csal_attrib_ptr, IID_CSAL_ATTRIB_ARRAY_INT32, (void**)(&csal_array_ptr) );

    TC_TEST( tf, 1 == csal_err );
    TC_TEST( tf, NULL == csal_array_ptr );

    csal_err = csal_attrib_cast( csal_attrib_ptr, IID_CSAL_ATTRIB_ARRAY_INT64, (void**)(&csal_array_ptr) );

    TC_TEST( tf, 1 == csal_err );
    TC_TEST( tf, NULL == csal_array_ptr );

    csal_err = csal_attrib_cast( csal_attrib_ptr, IID_CSAL_ATTRIB_ARRAY_UINT64, (void**)(&csal_array_ptr) );

    TC_TEST( tf, 1 == csal_err );
    TC_TEST( tf, NULL == csal_array_ptr );
#endif


    csal_attrib_destroy( (csal_attrib_t*)(at_arr) );

    return 0;
}
int test_int16_array( test_case_t* tf )
{

    uint64_t dims[2];
    uint64_t idxs[2];
    uint64_t ndims=2;

    int i,j;
    dims[0]=3;
    dims[1]=4;

    csal_attrib_array_int16_t* at_arr = csal_attrib_array_int16_create( dims, ndims );

    TC_TEST( tf, NULL != at_arr );
    TC_TEST( tf, CSAL_ATTR_ARRAY == csal_attrib_type( (csal_attrib_t*)at_arr ) );

    csal_attrib_t* csal_attrib_ptr = NULL;
    csal_attrib_array_int16_cast( at_arr, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr) );

    TC_TEST( tf, NULL!= csal_attrib_ptr );
    TC_TEST( tf, csal_true == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_ARRAY ) );

    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_BOOL ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_STRING ) );

    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            csal_attrib_array_int16_element_set( at_arr, idxs, ndims, i*j);
        }
    }

    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            TC_TEST( tf, i*j==csal_attrib_array_int16_element_get( at_arr, idxs, ndims) );
        }
    }

    csal_attrib_destroy( (csal_attrib_t*)(at_arr) );
    return 0;
}

int test_int32_array( test_case_t* tf )
{
    uint64_t dims[2];
    uint64_t idxs[2];
    uint64_t ndims=2;

    int i,j;
    int csal_err = 0;

    dims[0]=3;
    dims[1]=4;

    csal_attrib_array_int32_t* at_arr = csal_attrib_array_int32_create( dims, ndims );
    csal_attrib_array_int32_t* csal_array_ptr = NULL;


    TC_TEST( tf, NULL != at_arr );
    TC_TEST( tf, CSAL_ATTR_ARRAY == csal_attrib_type( (csal_attrib_t*)at_arr ) );

    csal_attrib_t* csal_attrib_ptr = NULL;
    csal_attrib_array_int32_cast( at_arr, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr) );

    TC_TEST( tf, NULL!= csal_attrib_ptr );
    TC_TEST( tf, csal_true == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_ARRAY ) );

    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_BOOL ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_STRING ) );


    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            csal_attrib_array_int32_element_set( at_arr, idxs, ndims, i*j);
        }
    }

    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            TC_TEST( tf, i*j==csal_attrib_array_int32_element_get( at_arr, idxs, ndims) );
        }
    }
#if 1 
    csal_err = csal_attrib_cast( (csal_attrib_t*)at_arr, IID_CSAL_ATTRIB_ARRAY, (void**)(&csal_array_ptr) );

    TC_TEST( tf, CSAL_ERR_NONE == csal_err );
    TC_TEST( tf, NULL != csal_array_ptr );
#endif

    csal_attrib_destroy( (csal_attrib_t*)(at_arr) );
    return 0;
}

int test_int64_array( test_case_t* tf )
{
    uint64_t dims[2];
    uint64_t idxs[2];
    uint64_t ndims=2;

    int i,j;

    dims[0]=3;
    dims[1]=4;

    csal_attrib_array_int64_t* at_arr = csal_attrib_array_int64_create( dims, ndims );

    TC_TEST( tf, NULL != at_arr );
    TC_TEST( tf, CSAL_ATTR_ARRAY == csal_attrib_type( (csal_attrib_t*)at_arr ) );

    csal_attrib_t* csal_attrib_ptr = NULL;
    csal_attrib_array_int64_cast( at_arr, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr) );

    TC_TEST( tf, NULL!= csal_attrib_ptr );
    TC_TEST( tf, csal_true == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_ARRAY ) );

    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_BOOL ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_STRING ) );


    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            csal_attrib_array_int64_element_set( at_arr, idxs, ndims, i*j);
        }
    }

    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            TC_TEST( tf, i*j==csal_attrib_array_int64_element_get( at_arr, idxs, ndims) );
        }
    }

    csal_attrib_destroy( (csal_attrib_t*)(at_arr) );
    return 0;
}





int test_uint8_array( test_case_t* tf )
{
    uint64_t dims[2];
    uint64_t idxs[2];
    uint64_t ndims=2;

    int i,j;

    dims[0]=3;
    dims[1]=4;

    csal_attrib_array_uint8_t* at_arr = csal_attrib_array_uint8_create( dims, ndims );


    TC_TEST( tf, NULL != at_arr );
    TC_TEST( tf, CSAL_ATTR_ARRAY == csal_attrib_type( (csal_attrib_t*)at_arr ) );

    csal_attrib_t* csal_attrib_ptr = NULL;
    csal_attrib_array_uint8_cast( at_arr, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr) );

    TC_TEST( tf, NULL!= csal_attrib_ptr );
    TC_TEST( tf, csal_true == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_ARRAY ) );

    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_BOOL ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_STRING ) );


    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            csal_attrib_array_uint8_element_set( at_arr, idxs, ndims, i*j);
        }
    }

    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            TC_TEST( tf, i*j==csal_attrib_array_uint8_element_get( at_arr, idxs, ndims) );
        }
    }

    csal_attrib_destroy( (csal_attrib_t*)(at_arr) );
    return 0;
}
int test_uint16_array( test_case_t* tf )
{
    uint64_t dims[2];
    uint64_t idxs[2];
    uint64_t ndims=2;

    int i,j;

    dims[0]=3;
    dims[1]=4;

    csal_attrib_array_uint16_t* at_arr = csal_attrib_array_uint16_create( dims, ndims );

    TC_TEST( tf, NULL != at_arr );
    TC_TEST( tf, CSAL_ATTR_ARRAY == csal_attrib_type( (csal_attrib_t*)at_arr ) );

    csal_attrib_t* csal_attrib_ptr = NULL;
    csal_attrib_array_uint16_cast( at_arr, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr) );

    TC_TEST( tf, NULL!= csal_attrib_ptr );
    TC_TEST( tf, csal_true == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_ARRAY ) );

    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_BOOL ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_STRING ) );


    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            csal_attrib_array_uint16_element_set( at_arr, idxs, ndims, i*j);
        }
    }

    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            TC_TEST( tf, i*j==csal_attrib_array_uint16_element_get( at_arr, idxs, ndims) );
        }
    }

    csal_attrib_destroy( (csal_attrib_t*)(at_arr) );
    return 0;
}

int test_uint32_array( test_case_t* tf )
{
    uint64_t dims[2];
    uint64_t idxs[2];
    uint64_t ndims=2;

    int i,j;

    dims[0]=3;
    dims[1]=4;

    csal_attrib_array_uint32_t* at_arr = csal_attrib_array_uint32_create( dims, ndims );

    TC_TEST( tf, NULL != at_arr );
    TC_TEST( tf, CSAL_ATTR_ARRAY == csal_attrib_type( (csal_attrib_t*)at_arr ) );

    csal_attrib_t* csal_attrib_ptr = NULL;
    csal_attrib_array_uint32_cast( at_arr, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr) );

    TC_TEST( tf, NULL!= csal_attrib_ptr );
    TC_TEST( tf, csal_true == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_ARRAY ) );

    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_BOOL ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_STRING ) );


    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            csal_attrib_array_uint32_element_set( at_arr, idxs, ndims, i*j);
        }
    }

    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            TC_TEST( tf, i*j==csal_attrib_array_uint32_element_get( at_arr, idxs, ndims) );
        }
    }

    csal_attrib_destroy( (csal_attrib_t*)(at_arr) );
    return 0;
}

int test_uint64_array( test_case_t* tf )
{
    uint64_t dims[2];
    uint64_t idxs[2];
    uint64_t ndims=2;

    int i,j;

    dims[0]=3;
    dims[1]=4;

    csal_attrib_array_uint64_t* at_arr = csal_attrib_array_uint64_create( dims, ndims );

    TC_TEST( tf, NULL != at_arr );
    TC_TEST( tf, CSAL_ATTR_ARRAY == csal_attrib_type( (csal_attrib_t*)at_arr ) );

    csal_attrib_t* csal_attrib_ptr = NULL;
    csal_attrib_array_uint64_cast( at_arr, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr) );

    TC_TEST( tf, NULL!= csal_attrib_ptr );
    TC_TEST( tf, csal_true == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_ARRAY ) );

    TC_TEST_EQUAL_INT( tf, csal_false, csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_BOOL ) );
    TC_TEST_EQUAL_INT( tf, csal_false, csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT8 ) );
    TC_TEST_EQUAL_INT( tf, csal_false, csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT16 ) );
    TC_TEST_EQUAL_INT( tf, csal_false, csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT32 ) );
    TC_TEST_EQUAL_INT( tf, csal_false, csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_STRING ) );


    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            csal_attrib_array_uint64_element_set( at_arr, idxs, ndims, i*j);
        }
    }

    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            TC_TEST( tf, i*j==csal_attrib_array_uint64_element_get( at_arr, idxs, ndims) );
        }
    }

    csal_attrib_destroy( (csal_attrib_t*)(at_arr) );
    return 0;
}




int test_float32_array( test_case_t* tf )
{
    uint64_t dims[2];
    uint64_t idxs[2];
    uint64_t ndims=2;

    int i,j;

    dims[0]=3;
    dims[1]=4;

    csal_attrib_array_float32_t* at_arr = csal_attrib_array_float32_create( dims, ndims );

    TC_TEST( tf, NULL != at_arr );
    TC_TEST( tf, CSAL_ATTR_ARRAY == csal_attrib_type( (csal_attrib_t*)at_arr ) );

    csal_attrib_t* csal_attrib_ptr = NULL;
    csal_attrib_array_float32_cast( at_arr, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr) );

    TC_TEST( tf, NULL!= csal_attrib_ptr );
    TC_TEST( tf, csal_true == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_ARRAY ) );

    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_BOOL ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_STRING ) );


    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            csal_attrib_array_float32_element_set( at_arr, idxs, ndims, i*j);
        }
    }

    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            TC_TEST_EQUAL_FLOAT( tf, i*j, csal_attrib_array_float32_element_get( at_arr, idxs, ndims) );
        }
    }

    csal_attrib_destroy( (csal_attrib_t*)(at_arr) );
    return 0;
}


int test_float64_array( test_case_t* tf )
{
    uint64_t dims[2];
    uint64_t idxs[2];
    uint64_t ndims=2;

    int i,j;

    dims[0]=3;
    dims[1]=4;

    csal_attrib_array_float64_t* at_arr = csal_attrib_array_float64_create( dims, ndims );

    csal_attrib_t* csal_attrib_ptr = NULL;
    csal_attrib_array_float64_cast( at_arr, IID_CSAL_ATTRIB, (void**)(&csal_attrib_ptr) );

    TC_TEST( tf, NULL!= csal_attrib_ptr );

    TC_TEST( tf, csal_true == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_ARRAY ) );

    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_BOOL ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_INT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT8 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT16 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_UINT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT32 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_FLOAT64 ) );
    TC_TEST( tf, csal_false == csal_attrib_is_type( csal_attrib_ptr, CSAL_ATTR_STRING ) );

    TC_TEST( tf, NULL != at_arr );
    TC_TEST( tf, CSAL_ATTR_ARRAY == csal_attrib_type( (csal_attrib_t*)at_arr ) );

    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            csal_attrib_array_float64_element_set( at_arr, idxs, ndims, i*j);
        }
    }

    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            idxs[0] = i;
            idxs[1] = j;

            TC_TEST_EQUAL_DOUBLE( tf, i*j, csal_attrib_array_float64_element_get( at_arr, idxs, ndims) );
        }
    }

    csal_attrib_destroy( (csal_attrib_t*)(at_arr) );

    return 0;
}


int test_string_array( test_case_t* tf )
{
    uint64_t dims[2];
    uint64_t idxs[2];
    uint64_t ndims=2;

    char buffer[ 256 ];

    int i,j;

    dims[0]=3;
    dims[1]=4;

    csal_attrib_array_string_t* at_arr = csal_attrib_array_string_create( dims, ndims );

    TC_TEST( tf, NULL != at_arr );
    TC_TEST( tf, CSAL_ATTR_ARRAY == csal_attrib_type( (csal_attrib_t*)at_arr ) );


    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            sprintf( buffer, "%d", i*j );
            idxs[0] = i;
            idxs[1] = j;

            csal_attrib_array_string_element_set( at_arr, idxs, ndims, buffer );
        }
    }

    for( i = 0; i < dims[0]; ++i )
    {
        for( j = 0; j < dims[1]; ++j )
        {
            sprintf( buffer, "%d", i*j );
            idxs[0] = i;
            idxs[1] = j;

            TC_TEST_EQUAL_INT( tf, CSAL_ERR_NONE,  strcmp( buffer, csal_attrib_array_string_element_get( at_arr, idxs, ndims) ) );
        }
    }

    csal_attrib_destroy( (csal_attrib_t*)(at_arr) );
    return 0;
}








int test_client( test_case_t* tf )
{
    int err = 0;
    int csal_err = 0;
    int summary = csal_false;

    csal_client_t* csal_client_ptr = NULL;
    csal_attrib_t* csal_attrib_ptr = NULL;

    csal_err = csal_client_create( &csal_client_ptr, "https://sal.jet.uk", csal_false );

    TC_TEST( tf, CSAL_ERR_NONE == csal_err );

    if( CSAL_ERR_NONE == csal_err )
    {
        csal_err = csal_client_authenticate( csal_client_ptr, "lkjlkj", "lkjlkj" );

        TC_TEST( tf, CSAL_ERR_NONE == csal_err );

        csal_err = csal_client_get( csal_client_ptr, "pulse/latest?object=full", summary, &csal_attrib_ptr );

        TC_TEST( tf, CSAL_ERR_NONE != csal_err );
        TC_TEST( tf, CSAL_CLIENT_ERR == csal_err );

        csal_client_destroy( csal_client_ptr );
    }

    return err;
}

















int main()
{
    test_suite_t _tf;
    test_suite_t* tf = &_tf;
    
    test_suite_init( tf, TEST_SUITE_CTRL_VERBOSE|TEST_SUITE_CTRL_EXIT_ON_FAIL );

    TS_TEST_CASE( tf, test_constants );

    TS_TEST_CASE( tf, test_construction );
#if 0
    test_branch( &tf );
#endif

    TS_TEST_CASE( tf, test_attrib_cast );

    if( 1 )
    {
        TS_TEST_CASE( tf, test_bool_array );
        TS_TEST_CASE( tf, test_int8_array );
        TS_TEST_CASE( tf, test_int16_array );
        TS_TEST_CASE( tf, test_int32_array );
        TS_TEST_CASE( tf, test_int64_array );

        TS_TEST_CASE( tf, test_uint8_array );
        TS_TEST_CASE( tf, test_uint16_array );
        TS_TEST_CASE( tf, test_uint32_array );
        TS_TEST_CASE( tf, test_uint64_array );

        TS_TEST_CASE( tf, test_float32_array );
        TS_TEST_CASE( tf, test_float64_array );
        TS_TEST_CASE( tf, test_string_array );
    }


    if( 1 )
        TS_TEST_CASE( tf, test_client );

    test_suite_finish( tf );

    return 0;
}