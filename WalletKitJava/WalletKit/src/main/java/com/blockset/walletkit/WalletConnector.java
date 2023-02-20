/*
 * Created by Bryan Goring <bryan.goring@brd.com> on 08/24/21.
 * Copyright (c) 2021 Breadwinner AG.  All right reserved.
 *
 * See the LICENSE file at the project root for license information.
 * See the CONTRIBUTORS file at the project root for a list of contributors.
 */
package com.blockset.walletkit;


import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.blockset.walletkit.errors.WalletConnectorError;
import com.blockset.walletkit.utility.CompletionHandler;
import com.google.common.base.Optional;
import com.google.common.base.Preconditions;

import java.util.Map;

public interface WalletConnector {

    /** Key
     *
     * A Key that may be used for signing if it contains a secret
     */
    interface Key {

        /**
         * Indicates that the key has a private key
         * @return True when the key has a secret
         */
        boolean hasSecret();
    }

    /** Signature
     *
     *  A signature holds the signature bytes in the form
     *  specific to the WalletConnector.
     */
    interface Signature {
        /**
         * Get signature data
         * @return The signature bytes
         */
        byte[] getData();
    }

    /** Digest
     *
     *  A Digest holds '32 hash bytes'
     */
    interface Digest {
        /**
         * Get digest data, typically 32 bytes
         * @return The digest data
         */
        byte[] getData32();
    }

    /** Transaction
     *
     */
    interface Transaction {

        /** Check if the transaction is signed
         *
         * @return True if signed
         */
        boolean isSigned();

        /** Gets the transaction serialization.
         *  This could be unsigned or unsigned according to the
         *  status indicated by ({@link #isSigned}
         *
         */
        Serialization getSerialization();

        /** Returns the transaction's identifier.
         *  This is optional and will exist if {@link #isSigned}
         */
        Optional<byte[]> getIdentifier();
    }

    /** Serialization
     *
     *  A serialization is a byte sequence representing an
     *  unsigned or signed transaction.
     *
     */
    interface Serialization {

        /**
         *  Get serialization data
         * @return the serialization bytes
         */
        byte[] getData();
    }

    /** Result class that can indicate Success with success object
     *  of a failure with a suitable failure (error) object.
     *
     *  Use static creation methods {@link Result#success} or {@link Result#failure}
     *  to create Result indications.
     * @param <Success>
     * @param <Failure>
     */
    class Result<Success, Failure> {
        private Success success;
        private Failure failure;

        private Result(   @Nullable Success success,
                          @Nullable Failure failure)  {
            this.success = success;
            this.failure = failure;
        }

        public @NonNull Success     getSuccess()   { return Preconditions.checkNotNull(success); }
        public @NonNull Failure     getFailure()   { return Preconditions.checkNotNull(failure); }

        public @Nullable Success    getSuccessOrNull() { return success; }
        public @Nullable Failure    getFailureOrNull() { return failure; }

        public boolean              isFailure()    { return failure != null; }
        public boolean              isSuccess()    { return !isFailure(); }

        /** Create a successful result with the object for celebration
         *
         * @param success A successful result object
         * @return A successfull result object
         */
        public static <Success> Result success(Success success) {
            return new Result(success, null);
        }

        /** Create a failed result with the object giving pause
         *
         * @param failure A successful result object
         * @return A failed result object
         */
        public static <Failure> Result failure(Failure failure) {
            return new Result(null, failure);
        }
    }

    /**
     * Creates a WalletConnector compatible signing Key
     * @param paperKey A BIP39 phrase
     * @return A Key object which is suitable for
     *        {@link WalletConnector#sign(byte[], Key, boolean)} or
     *        {@link WalletConnector#sign(Transaction, Key)}
     */
    Result<Key, WalletConnectorError>
    createKey( String paperKey );

    /** Composite wrapper of a {@link Digest} and {@link Signature}
     *  to allow returning them together.
     */
    class DigestAndSignaturePair {
        public final Digest digest;
        public final Signature signature;
        public DigestAndSignaturePair(Digest    digest,
                                      Signature signature) {
            this.digest = digest;
            this.signature = signature;
        }
    }

    /**
     * Sign arbitrary data
     *
     * @param message Arbitrary data to be signed
     * @param key: Private signing {@link Key} key
     * @param prefix Indicates to include optional prefix in the signature (TBD: may not need
     *               to be mandated)
     * @return The pair of the {@link Digest} and {@link Signature}, or on failure
     *         a {@link Result} composed with {@link WalletConnectorError.InvalidKeyForSigning}
     */
    Result<DigestAndSignaturePair, WalletConnectorError>
    sign(   byte[]       message,
            Key          key,
            boolean      prefix  );

    /**
     * Sign typed data. The typedData provided must firstly be a string containing a valid JSON object. Secondly,
     * the contents of the JSON must be typed data in the form suitable to the network on which the WalletConnector
     * operates (for example, with Ethereum networks, the typedData must be presented as EIP-712 structured data).
     *
     * This function designated for handling JSON-RPC `eth_signTypedData`
     *
     * @param typedData: The data to sign
     * @param key A private key
     *
     * @return * @return The pair of the {@link Digest} and {@link Signature}, or on failure
     *         a {@link Result} composed with
     *           {@link WalletConnectorError.InvalidKeyForSigning} if not having a private key, or
     *           {@link WalletConnectorError.InvalidJson} if the typedData is not a JSON, or
     *           {@link WalletConnectorError.InvalidTypedData} if the JSON is not a valid typed data
     *                                                         for the wallet connectors network
     */
    Result<DigestAndSignaturePair, WalletConnectorError>
    sign(   String       typedData,
            Key          key    );

    /**
     * Recover the public key
     *
     * @param digest The {@link Digest} digest
     * @param signature The corresponding {@link Signature} signature
     * @return On success, a public key or on failure a {@link Result} composed with
     *         {@link WalletConnectorError.UnknownEntity} to indicate the 'digest' or 'signature'
     *                                                    are not from 'self'
     *         {@link WalletConnectorError.UnrecoverableKey} if the signature was not produced by a
     *                                                       recoverable signing algorithm
     *
     */
     Result<Key, WalletConnectorError>
     recover (  Digest     digest,
                Signature  signature   );

    /**
     *  Create a serialization from arbitrary data, typically the data should
     *  be signed or unsigned transaction data, but no checks are performed.
     * @param data The data
     * @return A {@link Serialization} serialization
     */
    Serialization createSerialization(byte[] data);

    /**
     *  Create a Transaction from a wallet-connect-specific dictionary of arguments applicable to
     *  the connector's network.  For ETH the Dictionary keys are: {...}.  There are circumstances
     *  where the `arguments` do not specify the `NetworkFee` to use.  In this case the `defaultFee`
     *  will be used.
     *
     *  In practice, the caller cannot know if `arguments` does specify the fee and thus cannot tell
     *  if `defaultFee` must be provided.  The caller can simply provide a `defaultFee` always or
     *  the caller can look for `WalletConnectorError.missingFee` and then reinvoke this function
     *  with a non-nil fee.  [The User might need to be queried to select a `NetworkFee` and thus
     *  the caller might prefer to wait for `missingFee` before prompting the User.]
     *
     * @param arguments A Map (JSON-RPC-like) of create arguments
     * @param defaultFee If `arguments` does not include an argument that specifies the
     *                   network fee, then the `defaultFee` is used
     * @result An unsigned {@link Transaction} or {@link Result} composed with
     *         {@link WalletConnectorError.InvalidTransactionArguments} in case one or more missing
     *         required arguments, or,
     *         {@link WalletConnectorError.MissingFee} if the fee is neither among
     *         the transaction arguments nor provided via defaultFee
     */
    Result<Transaction, WalletConnectorError>
    createTransaction ( Map<String, String>     arguments,
                        @Nullable NetworkFee    defaultFee);

    /**
     * Create a Transaction from a signed or unsigned serialization. Creation of a
     * Transaction from the Serialization object implies that this Serialization data
     * conforms to the Network's conventions regarding serialization (i.e. it may
     * not be just arbitrary data).
     *
     * @param serialization A transaction serialization, signed or unsigned
     * @return On success, an unsigned or signed {@link Transaction}. On failure
     *         a {@link Result} composed with {@link WalletConnectorError.UnknownEntity}
     *         if the serialization is not from 'self'
     */
    Result<Transaction, WalletConnectorError>
    createTransaction ( Serialization serialization );

    /**
     * Sign a transaction
     *
     * This function is the 'sign' part of the ETH JSON-RPC `eth_sendTransaction` and
     * `eth_sendRawTransaction`.
     *
     * @param transaction The input transaction to be signed
     * @param key A private key
     * @return On success, a signed {@link Transaction} which will be distinct from the provided
     *         'transaction' argument. On failure, a {@link Result} with one of
     *         {@link WalletConnectorError.UnknownEntity} if the 'transaction' is not from 'self',
     *         or {@link WalletConnectorError.InvalidKeyForSigning} if 'key' is not private
     */
    Result<Transaction, WalletConnectorError>
    sign ( Transaction     transaction,
           Key             key         );

    /**
     * Send a transaction to the connector's network. As implied by the presence of a
     * CompletionHandler, this method executes asynchronously.
     *
     *
     * This function is the 'submit' part of the ETH JSON-RPC `eth_sendTransaction` and
     * `eth_sendRawTransaction`.
     *
     * @param transaction The transaction to be submitted
     * @param completion The handler to which {@link Transaction} result and potential errors
     *                   are directed. This method may indicate:
     *                   - {@link WalletConnectorError.SubmitFailed} to the completion if the `transaction` was not submitted
     *                   - {@link WalletConnectorError.UnknownEntity} if `transaction` is not from `self`, or,
     *                   - {@link WalletConnectorError.UnsignedTransaction} if `transaction` is not signed
     */
    void submit ( Transaction                                           transaction,
                  CompletionHandler<Transaction, WalletConnectorError>  completion  );
}
