--TEST--
Sodium length-taking functions throw a catchable MemoryError for a length too large for the memory limit
--EXTENSIONS--
sodium
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

$nonce = str_repeat("\0", SODIUM_CRYPTO_STREAM_NONCEBYTES);
$key = str_repeat("\0", SODIUM_CRYPTO_STREAM_KEYBYTES);

try {
    sodium_crypto_stream(2147483000, $nonce, $key);
} catch (MemoryError $e) {
    echo 'stream: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

$xnonce = str_repeat("\0", SODIUM_CRYPTO_STREAM_XCHACHA20_NONCEBYTES);
$xkey = str_repeat("\0", SODIUM_CRYPTO_STREAM_XCHACHA20_KEYBYTES);

try {
    sodium_crypto_stream_xchacha20(2147483000, $xnonce, $xkey);
} catch (MemoryError $e) {
    echo 'stream_xchacha20: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

$salt = str_repeat("\0", SODIUM_CRYPTO_PWHASH_SALTBYTES);

try {
    sodium_crypto_pwhash(
        2147483000,
        'password',
        $salt,
        SODIUM_CRYPTO_PWHASH_OPSLIMIT_INTERACTIVE,
        SODIUM_CRYPTO_PWHASH_MEMLIMIT_INTERACTIVE,
    );
} catch (MemoryError $e) {
    echo 'pwhash: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

var_dump(strlen(sodium_crypto_stream(32, $nonce, $key)));

?>
--EXPECT--
stream: MemoryError: The resulting string is too large to fit in the configured memory limit
stream_xchacha20: MemoryError: The resulting string is too large to fit in the configured memory limit
pwhash: MemoryError: The resulting string is too large to fit in the configured memory limit
int(32)
