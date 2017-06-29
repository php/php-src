--TEST--
UUID comparison antisymmetry
--DESCRIPTION--
if 𝑎 ≼ 𝑏 and 𝑏 ≼ 𝑎, then 𝑎 = 𝑏

https://en.wikipedia.org/wiki/Antisymmetric_relation
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

$a = UUID::fromBinary("\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1");
$b = UUID::fromBinary("\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1");

var_dump($a <= $b, $b <= $a, $a == $b);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
