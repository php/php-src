--TEST--
Long salts for DES in crypt()
--FILE--
<?php

echo "Standard DES" . \PHP_EOL;
$b = str_repeat("A", 124);
var_dump(crypt("A", $b));
$b = str_repeat("A", 125);
var_dump(crypt("A", $b));
$b = str_repeat("A", 4096);
var_dump(crypt("A", $b));

echo "Extended DES" . \PHP_EOL;
$b = str_repeat("A", 124);
var_dump(crypt("A", "_" . $b));
$b = str_repeat("A", 125);
var_dump(crypt("A", "_" . $b));
$b = str_repeat("A", 4096);
var_dump(crypt("A", "_" . $b));

?>
--EXPECT--
Standard DES
string(13) "AADp2/qSzuKtU"
string(13) "AADp2/qSzuKtU"
string(13) "AADp2/qSzuKtU"
Extended DES
string(20) "_AAAAAAAASn9YBRZgpQ."
string(20) "_AAAAAAAASn9YBRZgpQ."
string(20) "_AAAAAAAASn9YBRZgpQ."
