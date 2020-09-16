--TEST--
openssl key from zval leaks
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
?>
--FILE--
<?php
$cert = false;
class test {
    function __toString() {
        return "test object";
    }
}
$t = new test;

var_dump(openssl_x509_parse("foo"));

try {
    var_dump(openssl_x509_parse($t));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    openssl_x509_parse([]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

var_dump(openssl_x509_parse($cert));

try {
    openssl_x509_parse(new stdClass);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
bool(false)
bool(false)
openssl_x509_parse(): Argument #1 ($certificate) must be of type OpenSSLCertificate|string, array given
bool(false)
openssl_x509_parse(): Argument #1 ($certificate) must be of type OpenSSLCertificate|string, stdClass given
