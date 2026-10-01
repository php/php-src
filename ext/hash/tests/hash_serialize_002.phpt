--TEST--
Hash: serialize()/unserialize() with HASH_HMAC
--FILE--
<?php

$algos = hash_algos();
$non_crypto = ["adler32", "crc32", "crc32b", "crc32c", "fnv132", "fnv1a32", "fnv164", "fnv1a64", "joaat", "murmur3a", "murmur3c", "murmur3f", "xxh32", "xxh64", "xxh3", "xxh128"];
$key = "This is the key that I have";

foreach ($algos as $algo) {
    if (in_array($algo, $non_crypto)) {
        continue;
    }

    var_dump($algo);
    $ctx0 = hash_init($algo, HASH_HMAC, $key);
    try {
        $serial = serialize($ctx0);
        assert(is_string($serial));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

echo "Done\n";
?>
--EXPECT--
string(3) "md2"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(3) "md4"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(3) "md5"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(4) "sha1"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(6) "sha224"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(6) "sha256"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(6) "sha384"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "sha512/224"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "sha512/256"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(6) "sha512"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(8) "sha3-224"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(8) "sha3-256"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(8) "sha3-384"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(8) "sha3-512"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(9) "ripemd128"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(9) "ripemd160"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(9) "ripemd256"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(9) "ripemd320"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(9) "whirlpool"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "tiger128,3"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "tiger160,3"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "tiger192,3"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "tiger128,4"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "tiger160,4"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "tiger192,4"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(6) "snefru"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(9) "snefru256"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(4) "gost"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(11) "gost-crypto"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "haval128,3"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "haval160,3"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "haval192,3"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "haval224,3"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "haval256,3"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "haval128,4"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "haval160,4"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "haval192,4"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "haval224,4"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "haval256,4"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "haval128,5"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "haval160,5"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "haval192,5"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "haval224,5"
Exception: HashContext with HASH_HMAC option cannot be serialized
string(10) "haval256,5"
Exception: HashContext with HASH_HMAC option cannot be serialized
Done
