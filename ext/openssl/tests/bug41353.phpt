--TEST--
Bug #41353 (openssl_pkcs12_read() does not verify the type of the first arg)
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
?>
--FILE--
<?php

$a = 2;
openssl_pkcs12_read(1, $a, 1);

echo "Done\n";
?>
--EXPECTF--
Done
