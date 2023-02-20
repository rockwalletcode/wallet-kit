//
//  BR__Name__Address.c
//  WalletKitCore
//
//  Created by __USER__ on __DATE__.
//  Copyright © __YEAR__ Breadwinner AG. All rights reserved.
//
//  See the LICENSE file at the project root for license information.
//  See the CONTRIBUTORS file at the project root for a list of contributors.
//

#include "BR__Name__Address.h"
#include "BR__Name__Base.h"
#include "support/BRCrypto.h"
#include "support/BRBase58.h"

#include "blake2/blake2b.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <stdbool.h>

//
#define __NAME___ADDRESS_BYTES (1)


static uint8_t feeAddressBytes     [__NAME___ADDRESS_BYTES] = { 0 };
static uint8_t unknownAddressBytes [__NAME___ADDRESS_BYTES] = { 0 };


struct BR__Name__AddressRecord {
    uint8_t bytes[__NAME___ADDRESS_BYTES];
};

extern void
__name__AddressFree (BR__Name__Address address) {
    if (address) free(address);
}

BR__Name__Address
__name__AddressCreateFeeAddress() {
    BR__Name__Address address = calloc(1, sizeof(struct BR__Name__AddressRecord));
    memcpy(address->bytes, feeAddressBytes, __NAME___ADDRESS_BYTES);
    return address;
}

BR__Name__Address
__name__AddressCreateUnknownAddress() {
    BR__Name__Address address = calloc(1, sizeof(struct BR__Name__AddressRecord));
    memcpy(address->bytes, unknownAddressBytes, __NAME___ADDRESS_BYTES);
    return address;
}

extern bool
__name__AddressIsFeeAddress (BR__Name__Address address) {
    assert(address);
    return 0 == memcmp(address->bytes, feeAddressBytes, sizeof(feeAddressBytes));
}

extern bool
__name__AddressIsUnknownAddress (BR__Name__Address address)
{
    assert(address);
    return  0 == memcmp(address->bytes, unknownAddressBytes, sizeof(unknownAddressBytes));
}

extern char *
__name__AddressAsString (BR__Name__Address address) {
    assert(address);
    char * string = NULL;
    
    if (__name__AddressIsFeeAddress (address)) {
        string = strdup ("__fee__");
    } else if (__name__AddressIsUnknownAddress (address)) {
        string = strdup ("unknown");
    } else {
        ASSERT_UNIMPLEMENTED;
#if 0
        // address string is Base58check(prefix + Blake2b(publicKey) (20 bytes))
        size_t addressLen = BRBase58CheckEncode(NULL, 0, address->bytes, __NAME___ADDRESS_BYTES);
        string = calloc (1, addressLen);
        BRBase58CheckEncode(string, addressLen, address->bytes, __NAME___ADDRESS_BYTES);
#endif
    }
    return string;
}

extern BR__Name__Address
__name__AddressCreateFromKey (const uint8_t * pubKey, size_t pubKeyLen) {
    BR__Name__Address address = calloc(1, sizeof(struct BR__Name__AddressRecord));

    ASSERT_UNIMPLEMENTED;
#if 0
    uint8_t pkh[BLAKE20_BYTES];
    blake2b(pkh, sizeof(pkh), NULL, 0, pubKey, pubKeyLen);

    memcpy(address->bytes, __SYMBOL__TZ1_PREFIX, sizeof(__SYMBOL__TZ1_PREFIX));
    memcpy(address->bytes + sizeof(__SYMBOL__TZ1_PREFIX), pkh, sizeof(pkh));
#endif
    return address;
}

#if 0
static BR__Name__Address
__name__AddressCreateFromBytes (uint8_t * bytes, size_t length) {
    assert(bytes);
    assert(length == __NAME___ADDRESS_BYTES);

    ASSERT_UNIMPLEMENTED;
    BR__Name__Address address = calloc(1, sizeof(struct BR__Name__AddressRecord));
    memcpy(address->bytes, bytes, length);
    return address;
}
#endif

static BR__Name__Address
__name__AddressStringToAddress(const char *input) {

    ASSERT_UNIMPLEMENTED;

#if 0
    uint8_t bytes[__NAME___ADDRESS_BYTES];

    size_t length = BRBase58CheckDecode(bytes, sizeof(bytes), input);
    if (length != __NAME___ADDRESS_BYTES) {
        return NULL;
    }
    
    if (0 == memcmp(bytes, __SYMBOL__TZ1_PREFIX, sizeof(__SYMBOL__TZ1_PREFIX))
        || 0 == memcmp(bytes, __SYMBOL__TZ2_PREFIX, sizeof(__SYMBOL__TZ2_PREFIX))
        || 0 == memcmp(bytes, __SYMBOL__TZ3_PREFIX, sizeof(__SYMBOL__TZ3_PREFIX))
        || 0 == memcmp(bytes, __SYMBOL__KT_PREFIX, sizeof(__SYMBOL__KT_PREFIX))) {
        return __name__AddressCreateFromBytes(bytes, length);
    }
#endif
    return NULL;
}

extern BR__Name__Address
__name__AddressCreateFromString(const char * addressString, bool strict) {
    
    if (addressString == NULL || strlen(addressString) == 0) {
        return (strict
                ? NULL
                : __name__AddressCreateUnknownAddress ());
    } else if (strict) {
        return __name__AddressStringToAddress (addressString);
    } else if (strcmp(addressString, "unknown") == 0) {
        return __name__AddressCreateUnknownAddress ();
    } else if (strcmp(addressString, "__fee__") == 0) {
        return __name__AddressCreateFeeAddress ();
    } else {
        return __name__AddressStringToAddress (addressString);
    }
}

extern bool
__name__AddressEqual (BR__Name__Address a1, BR__Name__Address a2) {
    return 0 == memcmp (a1->bytes, a2->bytes, __NAME___ADDRESS_BYTES);
}

extern size_t
__name__AddressHashValue (BR__Name__Address address) {
    return *((size_t*) address->bytes);
}

extern BR__Name__Address
__name__AddressClone (BR__Name__Address address) {
    if (address) {
        BR__Name__Address clone = calloc(1, sizeof(struct BR__Name__AddressRecord));
        *clone = *address;
        return clone;
    }
    return NULL;
}

extern size_t
__name__AddressGetRawSize (BR__Name__Address address) {
    return __NAME___ADDRESS_BYTES;
}

extern void __name__AddressGetRawBytes (BR__Name__Address address, uint8_t *buffer, size_t bufferSize) {
    assert(buffer);
    assert(bufferSize >= __NAME___ADDRESS_BYTES);
    memcpy(buffer, address->bytes, __NAME___ADDRESS_BYTES);
}
