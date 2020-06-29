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
var_dump(openssl_x509_parse($t));
var_dump(openssl_x509_parse(array()));
var_dump(openssl_x509_parse($cert));
try {
    var_dump(openssl_x509_parse(new stdClass));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
Object of class stdClass could not be converted to string
