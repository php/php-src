--TEST--
openssl_cms_decrypt() tests
--EXTENSIONS--
openssl
--FILE--
<?php
$infile = __DIR__ . "/plain.txt";
$privkey = "file://" . __DIR__ . "/private_rsa_1024.key";
$encrypted = tempnam(sys_get_temp_dir(), "cms_dec_basic");
if ($encrypted === false)
    die("failed to get a temporary filename!");
$outfile = $encrypted . ".out";

$single_cert = "file://" . __DIR__ . "/cert.crt";
$headers = array("test@test", "testing openssl_cms_encrypt()");
$wrong = "wrong";
$empty = "";
$cipher = OPENSSL_CIPHER_AES_128_CBC;

openssl_cms_encrypt($infile, $encrypted, $single_cert, $headers, cipher_algo: $cipher);

var_dump(openssl_cms_decrypt($encrypted, $outfile, $single_cert, $privkey));
print("\nDecrypted text:\n");
readfile($outfile);
var_dump(openssl_cms_decrypt($encrypted, $outfile, openssl_x509_read($single_cert), $privkey));
var_dump(openssl_cms_decrypt($encrypted, $outfile, $single_cert, $wrong));
var_dump(openssl_cms_decrypt($encrypted, $outfile, $wrong, $privkey));
var_dump(openssl_cms_decrypt($encrypted, $outfile, null, $privkey));
var_dump(openssl_cms_decrypt($wrong, $outfile, $single_cert, $privkey));
var_dump(openssl_cms_decrypt($empty, $outfile, $single_cert, $privkey));
var_dump(openssl_cms_decrypt($encrypted, $empty, $single_cert, $privkey));
var_dump(openssl_cms_decrypt($encrypted, $outfile, $empty, $privkey));
var_dump(openssl_cms_decrypt($encrypted, $outfile, $single_cert, $empty));

if (file_exists($encrypted)) {
    echo "true\n";
    unlink($encrypted);
}
if (file_exists($outfile)) {
    echo "true\n";
    unlink($outfile);
}
?>
--EXPECTF--
bool(true)

Decrypted text:
Now is the winter of our discontent.
bool(true)

Warning: openssl_cms_decrypt(): Unable to get private key in %s on line %d
bool(false)

Warning: openssl_cms_decrypt(): X.509 Certificate cannot be retrieved in %s on line %d
bool(false)

Warning: openssl_cms_decrypt(): X.509 Certificate cannot be retrieved in %s on line %d
bool(false)
bool(false)
bool(false)
bool(false)

Warning: openssl_cms_decrypt(): X.509 Certificate cannot be retrieved in %s on line %d
bool(false)

Warning: openssl_cms_decrypt(): Unable to get private key in %s on line %d
bool(false)
true
true
