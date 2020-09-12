--TEST--
Bug #71475: openssl_seal() uninitialized memory usage
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
?>
--FILE--
<?php
$_ = str_repeat("A", 512);
try {
    openssl_seal($_, $_, $_, array_fill(0,64,0));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
DONE
--EXPECT--
openssl_seal() expects at least 5 arguments, 4 given
DONE
