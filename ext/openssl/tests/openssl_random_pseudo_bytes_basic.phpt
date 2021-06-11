--TEST--
openssl_random_pseudo_bytes() tests
--EXTENSIONS--
openssl
--FILE--
<?php
for ($i = 1; $i < 10; $i++) {
    var_dump(bin2hex(openssl_random_pseudo_bytes($i)));
}
?>
--EXPECTF--
string(2) "%s"
string(4) "%s"
string(6) "%s"
string(8) "%s"
string(10) "%s"
string(12) "%s"
string(14) "%s"
string(16) "%s"
string(18) "%s"
