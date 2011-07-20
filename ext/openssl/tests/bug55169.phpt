--TEST--
openssl_random_pseudo_bytes test
https://bugs.php.net/bug.php?id=55169
--SKIPIF--
<?php
if(!extension_loaded('openssl')) echo 'skip - requires openssl extension';
?>
--FILE--
<?php
for ($i = -1; $i <= 4; $i++) {
    $bytes = openssl_random_pseudo_bytes($i, $cstrong);
    $hex   = bin2hex($bytes);

    echo "Lengths: Bytes: $i and Hex: " . strlen($hex) . PHP_EOL;
    var_dump($hex);
    var_dump($cstrong);
    echo PHP_EOL;
}
?>
--EXPECTF--
Lengths: Bytes: -1 and Hex: 0
string(0) ""
NULL

Lengths: Bytes: 0 and Hex: 0
string(0) ""
NULL

Lengths: Bytes: 1 and Hex: 2
string(2) "%x"
bool(true)

Lengths: Bytes: 2 and Hex: 4
string(4) "%x"
bool(true)

Lengths: Bytes: 3 and Hex: 6
string(6) "%x"
bool(true)

Lengths: Bytes: 4 and Hex: 8
string(8) "%x"
bool(true)
