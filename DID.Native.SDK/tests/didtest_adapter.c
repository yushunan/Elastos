#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ela_did.h"
#include "spvadapter.h"
#include "didtest_adapter.h"

typedef struct TestDIDAdaptor {
    DIDAdapter base;
    SpvDidAdapter *impl;
    GetPasswordCallback *passwordCallback;
    char *walletDir;
    char *walletId;
} TestDIDAdaptor;

static int TestDIDAdaptor_CreateIdTransaction(DIDAdapter *_adapter, const char *payload, const char *memo)
{
    TestDIDAdaptor *adapter = (TestDIDAdaptor*)_adapter;
    const char *password;

    if (!adapter || !payload)
        return -1;

    password = adapter->passwordCallback((const char *)adapter->walletDir,
            (const char *)adapter->walletId);
    return SpvDidAdapter_CreateIdTransaction(adapter->impl, payload, memo, password);
}

static const char *TestAdapter_Resolver(DIDAdapter *_adapter, const char *did)
{
    TestDIDAdaptor *adapter = (TestDIDAdaptor*)_adapter;

    if (!adapter || !did)
        return NULL;

    return SpvDidAdapter_Resolve(adapter->impl, did);
}

DIDAdapter *TestAdapter_Create(const char *walletDir, const char *walletId,
        const char *network, const char *resolver, GetPasswordCallback *callback)
{
    TestDIDAdaptor *adapter;
    const char *password;

    if (!walletDir || !walletId || !callback)
        return NULL;

    adapter = (TestDIDAdaptor*)calloc(1, sizeof(TestDIDAdaptor));
    if (!adapter)
        return NULL;

    adapter->base.createIdTransaction = TestDIDAdaptor_CreateIdTransaction;
    adapter->base.resolve = TestAdapter_Resolver;

    adapter->impl = SpvDidAdapter_Create(walletDir, walletId, network, resolver);
    if (!adapter->impl) {
        free(adapter);
        return NULL;
    }

    adapter->passwordCallback = callback;
    adapter->walletDir = strdup(walletDir);
    adapter->walletId = strdup(walletId);

    return (DIDAdapter*)adapter;
}

void TestAdapter_Destroy(DIDAdapter *_adapter)
{
    TestDIDAdaptor *adapter = (TestDIDAdaptor*)_adapter;

    if (!adapter)
        return;

    if (adapter->impl)
        SpvDidAdapter_Destroy(adapter->impl);

    free(adapter->walletDir);
    free(adapter->walletId);

    free(adapter);
}


