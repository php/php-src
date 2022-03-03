--TEST--
openssl_cms_encrypt() tests
--EXTENSIONS--
openssl
--FILE--
<?php
$infile = __DIR__ . "/plain.txt";
$outfile = tempnam(sys_get_temp_dir(), "cms_enc_basic");
if ($outfile === false)
    die("failed to get a temporary filename!");
$outfile2 = $outfile . ".out";
$single_cert = "file://" . __DIR__ . "/cert.crt";
$privkey = "file://" . __DIR__ . "/private_rsa_1024.key";
$wrongkey = "file://" . __DIR__ . "/private_rsa_2048.key";
$multi_certs = array($single_cert, $single_cert);
$assoc_headers = array("To" => "test@test", "Subject" => "testing openssl_cms_encrypt()");
$headers = array("test@test", "testing openssl_cms_encrypt()");
$empty_headers = array();
$wrong = "wrong";
$empty = "";
$cipher = OPENSSL_CIPHER_AES_128_CBC;

var_dump(openssl_cms_encrypt($infile, $outfile, $single_cert, $headers, cipher_algo: $cipher));
var_dump(openssl_cms_encrypt($infile, $outfile, openssl_x509_read($single_cert), $headers, cipher_algo: $cipher));
var_dump(openssl_cms_decrypt($outfile, $outfile2, $single_cert, $privkey));
readfile($outfile2);
var_dump(openssl_cms_encrypt($infile, $outfile, $single_cert, $assoc_headers, cipher_algo: $cipher));
var_dump(openssl_cms_encrypt($infile, $outfile, $single_cert, $empty_headers, cipher_algo: $cipher));
var_dump(openssl_cms_encrypt($wrong, $outfile, $single_cert, $headers, cipher_algo: $cipher));
var_dump(openssl_cms_encrypt($empty, $outfile, $single_cert, $headers, cipher_algo: $cipher));
var_dump(openssl_cms_encrypt($infile, $empty, $single_cert, $headers, cipher_algo: $cipher));
var_dump(openssl_cms_encrypt($infile, $outfile, $wrong, $headers, cipher_algo: $cipher));
var_dump(openssl_cms_encrypt($infile, $outfile, $empty, $headers, cipher_algo: $cipher));
var_dump(openssl_cms_encrypt($infile, $outfile, $multi_certs, $headers, cipher_algo: $cipher));
var_dump(openssl_cms_encrypt($infile, $outfile, array_map('openssl_x509_read', $multi_certs), $headers, cipher_algo: $cipher));

if (file_exists($outfile)) {
    echo "true\n";
    unlink($outfile);
}
if (file_exists($outfile2)) {
    echo "true\n";
    unlink($outfile2);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
Now is the winter of our discontent.
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
true
true
