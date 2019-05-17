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
var_dump(openssl_x509_parse(new stdClass));

?>
--EXPECTF--
bool(false)
bool(false)
bool(false)
bool(false)

Recoverable fatal error: Object of class stdClass could not be converted to string in %sbug38261.php on line %d 
