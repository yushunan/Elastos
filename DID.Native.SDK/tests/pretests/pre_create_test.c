#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <CUnit/Basic.h>
#include "ela_did.h"
#include "loader.h"

static DIDDocument *doc;
static DID *did;

static void test_cred_fromjson(void)
{
    Credential *credential = Credential_FromJson(global_cred_string, did);
    CU_ASSERT_PTR_NOT_NULL_FATAL(credential);
    Credential_Destroy(credential);
}

static int cred_fromjson_test_suite_init(void)
{
    doc = DIDDocument_FromJson(global_did_string);
    if(!doc)
        return -1;

    did = DIDDocument_GetSubject(doc);

    return 0;
}

static int cred_fromjson_test_suite_cleanup(void)
{
    DIDDocument_Destroy(doc);
    return 0;
}

static CU_TestInfo cases[] = {
    { "test_cred_fromjson",            test_cred_fromjson    },
    { NULL,                            NULL                  }
};

static CU_SuiteInfo suite[] = {
    { "presentation create test",   cred_fromjson_test_suite_init,   cred_fromjson_test_suite_cleanup,     NULL, NULL, cases },
    {  NULL,                         NULL,                            NULL,                                 NULL, NULL, NULL  }
};


CU_SuiteInfo* pre_create_test_suite_info(void)
{
    return suite;
}
