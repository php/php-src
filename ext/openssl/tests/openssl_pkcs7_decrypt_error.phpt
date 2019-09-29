--TEST--
openssl_pkcs7_decrypt() and invalid parameters
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php

function myErrorHandler($errno, $errstr, $errfile, $errline) {
    var_dump($errstr);
}
set_error_handler("myErrorHandler");

$a = 1;
$b = 1;
$c = new stdclass;
$d = new stdclass;

try {
    var_dump(openssl_pkcs7_decrypt($a, $b, $c, $d));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($c);

var_dump(openssl_pkcs7_decrypt($b, $b, $b, $b));
var_dump(openssl_pkcs7_decrypt($a, $b, "", ""));
var_dump(openssl_pkcs7_decrypt($a, $b, true, false));
var_dump(openssl_pkcs7_decrypt($a, $b, 0, 0));

echo "Done\n";
?>
--EXPECT--
Object of class stdClass could not be converted to string
object(stdClass)#1 (0) {
}
string(66) "openssl_pkcs7_decrypt(): unable to coerce parameter 3 to x509 cert"
bool(false)
string(66) "openssl_pkcs7_decrypt(): unable to coerce parameter 3 to x509 cert"
bool(false)
string(66) "openssl_pkcs7_decrypt(): unable to coerce parameter 3 to x509 cert"
bool(false)
string(66) "openssl_pkcs7_decrypt(): unable to coerce parameter 3 to x509 cert"
bool(false)
Done
