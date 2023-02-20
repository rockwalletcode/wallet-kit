//
//  WKWalletManager__SYMBOL__.c
//  WalletKitCore
//
//  Created by __USER__ on __DATE__.
//  Copyright © __YEAR__ Breadwinner AG. All rights reserved.
//
//  See the LICENSE file at the project root for license information.
//  See the CONTRIBUTORS file at the project root for a list of contributors.
//
#include "WK__SYMBOL__.h"

#include "walletkit/WKAccountP.h"
#include "walletkit/WKNetworkP.h"
#include "walletkit/WKKeyP.h"
#include "walletkit/WKClientP.h"
#include "walletkit/WKWalletP.h"
#include "walletkit/WKAmountP.h"
#include "walletkit/WKWalletManagerP.h"
#include "walletkit/WKFileService.h"
#include "walletkit/WKHashP.h"

#include "__name__/BR__Name__Account.h"


// MARK: - Events

static const BREventType *__symbol__EventTypes[] = {
    WK_CLIENT_EVENT_TYPES
};

static const unsigned int
__symbol__EventTypesCount = (sizeof (__symbol__EventTypes) / sizeof (BREventType*));

// MARK: - Handlers

static WKWalletManager
wkWalletManagerCreate__SYMBOL__ (WKWalletManagerListener listener,
                              WKClient client,
                              WKAccount account,
                              WKNetwork network,
                              WKSyncMode mode,
                              WKAddressScheme scheme,
                              const char *path) {
    return wkWalletManagerAllocAndInit (sizeof (struct WKWalletManager__SYMBOL__Record),
                                            wkNetworkGetType(network),
                                            listener,
                                            client,
                                            account,
                                            network,
                                            scheme,
                                            path,
                                            WK_CLIENT_REQUEST_USE_TRANSFERS,
                                            NULL,
                                            NULL);
}

static void
wkWalletManagerRelease__SYMBOL__ (WKWalletManager manager) {
}

static BRFileService
crytpWalletManagerCreateFileService__SYMBOL__ (WKWalletManager manager,
                                        const char *basePath,
                                        const char *currency,
                                        const char *network,
                                        BRFileServiceContext context,
                                        BRFileServiceErrorHandler handler) {
    return fileServiceCreateFromTypeSpecifications (basePath, currency, network,
                                                    context, handler,
                                                    wkFileServiceSpecificationsCount,
                                                    wkFileServiceSpecifications);
}

static const BREventType **
wkWalletManagerGetEventTypes__SYMBOL__ (WKWalletManager manager,
                                     size_t *eventTypesCount) {
    assert (NULL != eventTypesCount);
    *eventTypesCount = __symbol__EventTypesCount;
    return __symbol__EventTypes;
}

static WKClientP2PManager
crytpWalletManagerCreateP2PManager__SYMBOL__ (WKWalletManager manager) {
    // not supported
    return NULL;
}

static WKBoolean
wkWalletManagerSignTransactionWithSeed__SYMBOL__ (WKWalletManager manager,
                                               WKWallet wallet,
                                               WKTransfer transfer,
                                               UInt512 seed) {
    BR__Name__Account __symbol__Account = wkAccountGetAs__SYMBOL__ (manager->account);
    BR__Name__Transaction __symbol__Transaction = wkTransferCoerce__SYMBOL__(transfer)->__symbol__Transaction;

    size_t serializationSize = 0;

    if (__symbol__Transaction) {
        uint8_t *serialization = __name__TransactionSerializeForSubmission (__symbol__Transaction, __symbol__Account, seed, &serializationSize);
        free (serialization);
    }

    return AS_WK_BOOLEAN (serializationSize > 0);
}

static WKBoolean
wkWalletManagerSignTransactionWithKey__SYMBOL__ (WKWalletManager manager,
                                              WKWallet wallet,
                                              WKTransfer transfer,
                                              WKKey key) {
    assert(0);
    return WK_FALSE;
}

static WKAmount
wkWalletManagerEstimateLimit__SYMBOL__ (WKWalletManager manager,
                                     WKWallet  wallet,
                                     WKBoolean asMaximum,
                                     WKAddress target,
                                     WKNetworkFee networkFee,
                                     WKBoolean *needEstimate,
                                     WKBoolean *isZeroIfInsuffientFunds,
                                     WKUnit unit) {
#if 0
    *needEstimate = asMaximum;
#endif

    return (WK_TRUE == asMaximum
            ? wkWalletGetBalance (wallet)        // Maximum is balance - fees 'needEstimate'
            : wkAmountCreateInteger (0, unit));  // No minimum
}

static WKFeeBasis
wkWalletManagerEstimateFeeBasis__SYMBOL__ (WKWalletManager manager,
                                        WKWallet wallet,
                                        WKCookie cookie,
                                        WKAddress target,
                                        WKAmount amount,
                                        WKNetworkFee networkFee,
                                        size_t attributesCount,
                                        OwnershipKept WKTransferAttribute *attributes) {
#if 0
    BR__Name__Amount mutezPerByte = __name__MutezCreate (networkFee->pricePerCostFactor) / 1000; // given as nanotez/byte
    BR__Name__FeeBasis __symbol__FeeBasis = __name__DefaultFeeBasis (mutezPerByte);
    WKFeeBasis feeBasis = wkFeeBasisCreateAs__SYMBOL__ (networkFee->pricePerCostFactorUnit, __symbol__FeeBasis);

    WKCurrency currency = wkAmountGetCurrency (amount);
    WKTransfer transfer = wkWalletCreateTransfer__SYMBOL__ (wallet,
                                                               target,
                                                               amount,
                                                               feeBasis,
                                                               attributesCount,
                                                               attributes,
                                                               currency,
                                                               wallet->unit,
                                                               wallet->unitForFee);

    wkCurrencyGive(currency);
    
    // serialize the transaction for fee estimation payload
    BR__Name__Hash lastBlockHash = wkHashAs__SYMBOL__ (wkNetworkGetVerifiedBlockHash (manager->network));
    BR__Name__Account __symbol__Account = wkAccountGetAs__SYMBOL__ (manager->account);
    BR__Name__Transaction tid = __name__TransactionGetTransaction (wkTransferCoerce__SYMBOL__(transfer)->__symbol__Transaction);
    bool needsReveal = (__NAME___OP_TRANSACTION == __name__TransactionGetOperationKind(tid)) && wkWalletNeedsReveal__SYMBOL__(wallet);
    
    __name__TransactionSerializeForFeeEstimation(tid,
                                              __symbol__Account,
                                              lastBlockHash,
                                              needsReveal);
    
    // serialized tx size is needed for fee estimation
    wkFeeBasisGive (feeBasis);
    feeBasis = wkFeeBasisCreateAs__SYMBOL__ (networkFee->pricePerCostFactorUnit, __name__TransactionGetFeeBasis(tid));

    wkClientQRYEstimateTransferFee (manager->qryManager,
                                        cookie,
                                        transfer,
                                        networkFee,
                                        feeBasis);

    wkTransferGive (transfer);
    wkFeeBasisGive (feeBasis);

    // Require QRY with cookie - made above
    return NULL;
#endif
    BR__Name__FeeBasis __symbol__FeeBasis = __name__FeeBasisCreate ();
    return wkFeeBasisCreateAs__SYMBOL__ (wallet->unitForFee, __symbol__FeeBasis);
}

static void
wkWalletManagerRecoverTransfersFromTransactionBundle__SYMBOL__ (WKWalletManager manager,
                                                             OwnershipKept WKClientTransactionBundle bundle) {
    // Not __SYMBOL__ functionality
    assert (0);
}

static const char *
cwmLookupAttributeValueForKey (const char *key, size_t count, const char **keys, const char **vals) {
    for (size_t index = 0; index < count; index++)
        if (0 == strcasecmp (key, keys[index]))
            return vals[index];
    return NULL;
}

static uint64_t
cwmParseUInt64 (const char *string, bool *error) {
    if (!string) { *error = true; return 0; }
    return strtoull(string, NULL, 0);
}

static void
wkWalletManagerRecoverTransferFromTransferBundle__SYMBOL__ (WKWalletManager manager,
                                                         OwnershipKept WKClientTransferBundle bundle) {
    // The wallet holds currency transfers
    WKWallet wallet = wkWalletManagerGetWallet (manager);

    BR__Name__Account __symbol__Account = wkAccountGetAs__SYMBOL__ (manager->account);

    BR__Name__Amount __symbol__Amount;
    sscanf(bundle->amount, "%" PRIu64, &__symbol__Amount);

    BR__Name__Amount __symbol__Fee = 0;
    if (NULL != bundle->fee) sscanf(bundle->fee, "%" PRIu64, &__symbol__Fee);

    // Get the `source` and `target` addresses.  We'll only use `source` if we need to create a
    // transfer; we'll use `target` both if a transfer is created and to identify a pre-existing
    // transfer held by wallet.
    BR__Name__Address __symbol__Target = __name__AddressCreateFromString (bundle->to,   false);
    BR__Name__Address __symbol__Source = __name__AddressCreateFromString (bundle->from, false);

    WKAddress target = wkAddressCreateAs__SYMBOL__ (__symbol__Target);
    WKAddress source = wkAddressCreateAs__SYMBOL__ (__symbol__Source);

    // A transaction may include a "burn" transfer to target address 'unknown' in addition to the
    // normal transfer, both sharing the same hash. Typically occurs when sending to an un-revealed
    // address.  It must be included since the burn amount is subtracted from wallet balance, but
    // is not considered a normal fee.
    WKHash         hash = wkHashCreateFromStringAs__SYMBOL__ (bundle->hash);
    WKTransfer transfer = wkWalletGetTransferByHashOrUIDS (wallet, hash, bundle->uids);

    BR__Name__FeeBasis __symbol__FeeBasis = __name__FeeBasisCreate();
    WKFeeBasis  feeBasis = wkFeeBasisCreateAs__SYMBOL__ (wallet->unitForFee, __symbol__FeeBasis);

    WKTransferState state = wkClientTransferBundleGetTransferState (bundle, feeBasis);

    if (NULL != transfer) {
        wkTransferSetUids  (transfer, bundle->uids);
        wkTransferSetState (transfer, state);
    }
    else {
        BR__Name__Transaction __symbol__Transaction = __name__TransactionCreate (__symbol__Source,
                                                                                 __symbol__Target,
                                                                                 __symbol__Amount,
                                                                                 __symbol__FeeBasis);

        transfer = wkTransferCreateAs__SYMBOL__ (wallet->listenerTransfer,
                                                  bundle->uids,
                                                  wallet->unit,
                                                  wallet->unitForFee,
                                                  state,
                                                  __symbol__Account,
                                                  __symbol__Transaction);
        wkWalletAddTransfer (wallet, transfer);
    }

    wkWalletManagerRecoverTransferAttributesFromTransferBundle (wallet, transfer, bundle);
    
    wkTransferGive (transfer);
    wkHashGive (hash);

    wkAddressGive (source);
    wkAddressGive (target);

    wkFeeBasisGive (feeBasis);
    wkTransferStateGive (state);

    wkWalletGive (wallet);
}

static WKFeeBasis
wkWalletManagerRecoverFeeBasisFromFeeEstimate__SYMBOL__ (WKWalletManager cwm,
                                                  WKTransfer transfer,
                                                  WKNetworkFee networkFee,
                                                  double costUnits,
                                                  size_t attributesCount,
                                                  OwnershipKept const char **attributeKeys,
                                                  OwnershipKept const char **attributeVals) {
#if 0
    bool parseError;
    
    int64_t gasUsed = (int64_t) cwmParseUInt64 (cwmLookupAttributeValueForKey ("consumed_gas", attributesCount, attributeKeys, attributeVals), &parseError);
    int64_t storageUsed = (int64_t) cwmParseUInt64 (cwmLookupAttributeValueForKey ("storage_size", attributesCount, attributeKeys, attributeVals), &parseError);
    int64_t counter = (int64_t) cwmParseUInt64 (cwmLookupAttributeValueForKey ("counter", attributesCount, attributeKeys, attributeVals), &parseError);
    // increment counter
    counter += 1;
    // add 10% padding to gas/storage limits
    gasUsed = (int64_t)(gasUsed * 1.1);
    storageUsed = (int64_t)(storageUsed * 1.1);
    BR__Name__Amount mutezPerKByte = __name__MutezCreate (networkFee->pricePerCostFactor); // given as nanotez/byte
    
    // get the serialized txn size from the estimation payload
    double sizeInKBytes = wkFeeBasisCoerce__SYMBOL__(initialFeeBasis)->__symbol__FeeBasis.u.initial.sizeInKBytes;

    BR__Name__FeeBasis feeBasis = __name__FeeBasisCreateEstimate (mutezPerKByte,
                                                            sizeInKBytes,
                                                            gasUsed,
                                                            storageUsed,
                                                            counter);
#endif

    BR__Name__FeeBasis __symbol__FeeBasis = __name__FeeBasisCreate ();
    return wkFeeBasisCreateAs__SYMBOL__ (networkFee->pricePerCostFactorUnit, __symbol__FeeBasis);
}

extern WKWalletSweeperStatus
wkWalletManagerWalletSweeperValidateSupported__SYMBOL__ (WKWalletManager manager,
                                                      WKWallet wallet,
                                                      WKKey key) {
    return WK_WALLET_SWEEPER_UNSUPPORTED_CURRENCY;
}

extern WKWalletSweeper
wkWalletManagerCreateWalletSweeper__SYMBOL__ (WKWalletManager manager,
                                           WKWallet wallet,
                                           WKKey key) {
    // not supported
    return NULL;
}

static WKWallet
wkWalletManagerCreateWallet__SYMBOL__ (WKWalletManager manager,
                                    WKCurrency currency,
                                    Nullable OwnershipKept BRArrayOf(WKClientTransactionBundle) transactions,
                                    Nullable OwnershipKept BRArrayOf(WKClientTransferBundle) transfers) {
    BR__Name__Account __symbol__Account = wkAccountGetAs__SYMBOL__ (manager->account);

    // Create the primary WKWallet
    WKNetwork  network       = manager->network;
    WKUnit     unitAsBase    = wkNetworkGetUnitAsBase    (network, currency);
    WKUnit     unitAsDefault = wkNetworkGetUnitAsDefault (network, currency);
    
    WKWallet wallet = wkWalletCreateAs__SYMBOL__ (manager->listenerWallet,
                                                     unitAsDefault,
                                                     unitAsDefault,
                                                     __symbol__Account);
    wkWalletManagerAddWallet (manager, wallet);
    
    // TODO:__SYMBOL__ load transfers from fileService
    
    wkUnitGive (unitAsDefault);
    wkUnitGive (unitAsBase);
    
    return wallet;
}

WKWalletManagerHandlers wkWalletManagerHandlers__SYMBOL__ = {
    wkWalletManagerCreate__SYMBOL__,
    wkWalletManagerRelease__SYMBOL__,
    crytpWalletManagerCreateFileService__SYMBOL__,
    wkWalletManagerGetEventTypes__SYMBOL__,
    crytpWalletManagerCreateP2PManager__SYMBOL__,
    wkWalletManagerCreateWallet__SYMBOL__,
    wkWalletManagerSignTransactionWithSeed__SYMBOL__,
    wkWalletManagerSignTransactionWithKey__SYMBOL__,
    wkWalletManagerEstimateLimit__SYMBOL__,
    wkWalletManagerEstimateFeeBasis__SYMBOL__,
    NULL, // WKWalletManagerSaveTransactionBundleHandler
    NULL, // WKWalletManagerSaveTransactionBundleHandler
    wkWalletManagerRecoverTransfersFromTransactionBundle__SYMBOL__,
    wkWalletManagerRecoverTransferFromTransferBundle__SYMBOL__,
    wkWalletManagerRecoverFeeBasisFromFeeEstimate__SYMBOL__,
    wkWalletManagerWalletSweeperValidateSupported__SYMBOL__,
    wkWalletManagerCreateWalletSweeper__SYMBOL__
};
