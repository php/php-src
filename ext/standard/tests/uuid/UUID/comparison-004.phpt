--TEST--
UUID comparison toset
--DESCRIPTION--
only one of 𝑎 ≺ 𝑏, 𝑎 = 𝑏, or 𝑎 ≻ 𝑏 is true

https://en.wikipedia.org/wiki/Toset
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

$a = UUID::fromBinary("\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1");
$b = UUID::fromBinary("\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2");
echo var_dump($a < $b, $a == $b, $a > $b), "\n";

$a = UUID::fromBinary("\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1");
$b = UUID::fromBinary("\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1");
echo var_dump($a < $b, $a == $b, $a > $b), "\n";

$a = UUID::fromBinary("\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2");
$b = UUID::fromBinary("\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1");
echo var_dump($a < $b, $a == $b, $a > $b), "\n";

?>
--EXPECT--
bool(true)
bool(false)
bool(false)

bool(false)
bool(true)
bool(false)

bool(false)
bool(false)
bool(true)
