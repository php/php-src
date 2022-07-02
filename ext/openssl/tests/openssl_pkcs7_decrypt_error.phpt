--TEST--
openssl_pkcs7_decrypt() and invalid parameters
--EXTENSIONS--
openssl
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
string(62) "openssl_pkcs7_decrypt(): X.509 Certificate cannot be retrieved"
bool(false)
string(62) "openssl_pkcs7_decrypt(): X.509 Certificate cannot be retrieved"
bool(false)
string(62) "openssl_pkcs7_decrypt(): X.509 Certificate cannot be retrieved"
bool(false)
string(62) "openssl_pkcs7_decrypt(): X.509 Certificate cannot be retrieved"
bool(false)
Done
