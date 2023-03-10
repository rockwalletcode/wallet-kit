//
//  WKKey.swift
//  WalletKit
//
//  Created by Ed Gamble on 7/22/19.
//  Copyright © 2019 Breadwinner AG. All rights reserved.
//
//  See the LICENSE file at the project root for license information.
//  See the CONTRIBUTORS file at the project root for a list of contributors.
//
import Foundation // Data
import WalletKitCore

public final class Key {
    static public var wordList: [String]?

    ///
    /// Check if a private key `string` is a valid passphrase-protected private key. The string
    /// must be BIP38 format.
    ///
    static public func isProtected(asPrivate string: String) -> Bool {
        return WK_TRUE == wkKeyIsProtectedPrivate (string)
    }

    ///
    /// Create `Key` from a BIP-39 phrase
    ///
    /// - Parameters:
    ///   - phrase: A 12 word phrase (aka paper key)
    ///   - words: Official BIP-39 list of words, with 2048 entries, in the language for `phrase`
    ///
    /// - Returns: A Key, if the phrase if valid
    ///
    static public func createFrom (phrase: String, words: [String]? = wordList) -> Key? {
        guard var words = words?.map ({ UnsafePointer<Int8> (strdup($0)) }) else { return nil }
        defer { words.forEach { wkMemoryFree (UnsafeMutablePointer (mutating: $0)) } }

        return wkKeyCreateFromPhraseWithWords (phrase, &words)
            .map { Key (core: $0)}
    }

    ///
    /// Create `Key` from `string` using the passphrase to decrypt it. The string must be BIP38
    /// format. Different cryptocurrencies have different implementations; this function will
    /// look for a valid string using BITCOIN mainnet and BITCOIN testnet.
    ///
    /// - Parameter string
    /// - Parameter passphrase
    ///
    /// - Returns: A Key if one exists
    ///
    static public func createFromString (asPrivate string: String, withPassphrase: String) -> Key? {
        return wkKeyCreateFromStringProtectedPrivate (string, withPassphrase)
            .map { Key (core: $0) }
    }

    ///
    /// Create `Key` from `string`.  The string must be wallet import format (WIF), mini private
    /// key format, or hex string for example: 5HxWvvfubhXpYYpS3tJkw6fq9jE9j18THftkZjHHfmFiWtmAbrj
    /// Different cryptocurrencies have different formats; this function will look for a valid
    /// string using BITCOIN mainnet and BITCOIN testnet.
    ///
    /// - Parameter string
    ///
    /// - Returns: A Key if one exists
    ///
    static public func createFromString (asPrivate string: String) -> Key? {
        return wkKeyCreateFromStringPrivate (string)
            .map { Key (core: $0) }
    }

    ///
    /// Create `Key`, as a public key, from `string`.  The string must be the hex-encoded
    /// DER-encoded public key that is produced by `encodeAsPublic`
    ///
    /// - Parameter string:
    ///
    /// - Returns: A Key, if one exists
    ///
    static public func createFromString (asPublic string: String) -> Key? {
        return wkKeyCreateFromStringPublic (string)
            .map { Key (core: $0) }
    }


    static public func createForPigeonFrom (key: Key, nonce: Data) -> Key {
        let nonceCount = nonce.count
        var nonce = nonce
        return nonce.withUnsafeMutableBytes { (nonceBytes: UnsafeMutableRawBufferPointer) -> Key in
            let nonceAddr  = nonceBytes.baseAddress?.assumingMemoryBound(to: UInt8.self)

            return wkKeyCreateForPigeon (key.core, nonceAddr, nonceCount)
                .map { Key (core: $0) }!
        }
    }

    static public func createForBIP32ApiAuth (phrase: String, words: [String]? = wordList) -> Key? {
        guard var words = words?.map ({ UnsafePointer<Int8> (strdup($0)) }) else { return nil }
        defer { words.forEach { wkMemoryFree (UnsafeMutablePointer (mutating: $0)) } }

        return wkKeyCreateForBIP32ApiAuth (phrase, &words)
            .map { Key (core: $0) }
    }

    static public func createForBIP32BitID (phrase: String, index: Int, uri:String, words: [String]? = wordList) -> Key? {
        guard var words = words?.map ({ UnsafePointer<Int8> (strdup($0)) }) else { return nil }
        defer { words.forEach { wkMemoryFree (UnsafeMutablePointer (mutating: $0)) } }

        return wkKeyCreateForBIP32BitID (phrase, Int32(index), uri, &words)
            .map { Key (core: $0) }
    }

    // The Core representation
    internal let core: WKKey

    deinit { wkKeyGive (core) }

    public var hasSecret: Bool {
        return 1 == wkKeyHasSecret  (self.core)
    }

    /// Return the WIF-encoded private key
    public var encodeAsPrivate: String {
        return asUTF8String(wkKeyEncodePrivate(self.core), true)
    }

    /// Return the hex-encoded, DER-encoded public key
    public var encodeAsPublic: String {
        return asUTF8String (wkKeyEncodePublic (self.core), true)
    }

    public typealias Secret = (  // 32 bytes
        UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8,
        UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8,
        UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8,
        UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8)

    public var secret: Secret {
        return wkKeyGetSecret (self.core).data
    }
    
    ///
    /// Check if `self` and `that` have an identical public key
    ///
    /// - Parameter that: the other CryptoKey
    ///
    /// - Returns: If identical `true`; otherwise `false`
    ///
    public func publicKeyMatch (_ that: Key) -> Bool {
        return 1 == wkKeyPublicMatch (core, that.core)
    }

    internal func privateKeyMatch (_ that: Key) -> Bool {
        return 1 == wkKeySecretMatch (core, that.core)
    }

    ///
    /// Initialize based on a Core BRKey - the provided BRKey might be private+public or just
    /// a public key (such as one that is recovered from the signature.
    ///
    /// - Parameter core: The Core representaion
    ///
    internal init (core: WKKey) {
        self.core = core
        wkKeyProvidePublicKey (core, 0, 0)
    }

    ///
    /// Initialize based on `secret` to produce a private+public key pair
    ///
    /// - Parameter secret: the secret
    ///
    internal convenience init (secret: Secret) {
        self.init (core: wkKeyCreateFromSecret (WKSecret.init(data: secret)))
    }
}
