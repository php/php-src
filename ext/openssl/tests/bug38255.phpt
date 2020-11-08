--TEST--
openssl key from zval leaks
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
?>
--FILE--
<?php
$pub_key_id = false;
$signature = '';
$ok = openssl_verify("foo", $signature, $pub_key_id, OPENSSL_ALGO_MD5);

class test {
    function __toString() {
        return "test object";
    }
}
$t = new test;


var_dump(openssl_verify("foo", $signature, $pub_key_id, OPENSSL_ALGO_MD5));
var_dump(openssl_verify("foo", $t, $pub_key_id, OPENSSL_ALGO_MD5));

echo "Done\n";

?>
--EXPECTF--
Warning: openssl_verify(): Supplied key param cannot be coerced into a public key in %s on line %d

Warning: openssl_verify(): Supplied key param cannot be coerced into a public key in %s on line %d
bool(false)

Warning: openssl_verify(): Supplied key param cannot be coerced into a public key in %s on line %d
bool(false)
Done
