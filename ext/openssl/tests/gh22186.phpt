--TEST--
GH-22186 (Heap buffer overflow in openssl_encrypt with AES-WRAP-PAD)
--EXTENSIONS--
openssl
--SKIPIF--
<?php
/* openssl_get_cipher_methods() enumerates provider ciphers, but openssl_encrypt()
 * resolves names via the legacy EVP_get_cipherbyname(), so on some builds the
 * cipher is listed yet not usable. Probe the actual call path instead. */
if (!@openssl_encrypt("test", "aes-128-wrap-pad", str_repeat("k", 16),
        OPENSSL_RAW_DATA | OPENSSL_DONT_ZERO_PAD_KEY, str_repeat("\0", 4))) {
    die('skip aes-128-wrap-pad not usable on this OpenSSL build');
}
?>
--FILE--
<?php
$pass = str_repeat("k", 16);
$iv = str_repeat("\0", 4);

for ($i = 1; $i < 258; $i++) {
    $data = str_repeat("a", $i);
    $enc = openssl_encrypt($data, 'aes-128-wrap-pad', $pass, OPENSSL_RAW_DATA | OPENSSL_DONT_ZERO_PAD_KEY, $iv);
    $dec = openssl_decrypt($enc, 'aes-128-wrap-pad', $pass, OPENSSL_RAW_DATA | OPENSSL_DONT_ZERO_PAD_KEY, $iv);
    if ($dec !== $data) {
        die("mismatch at $i\n");
    }
}

echo "done\n";
?>
--EXPECT--
done
