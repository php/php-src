--TEST--
openssl_random_pseudo_bytes() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
for ($i = 0; $i < 10; $i++) {
    var_dump(bin2hex(openssl_random_pseudo_bytes($i, $strong)));
}

?>
--EXPECTF--
string(0) ""
string(2) "%s"
string(4) "%s"
string(6) "%s"
string(8) "%s"
string(10) "%s"
string(12) "%s"
string(14) "%s"
string(16) "%s"
string(18) "%s"
