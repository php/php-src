--TEST--
UUID comparison transitivity
--DESCRIPTION--
if 𝑎 ≼ 𝑏 and 𝑏 ≼ 𝑐, then 𝑎 ≼ 𝑐

https://en.wikipedia.org/wiki/Transitive_relation
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

$a = UUID::fromBinary("\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1");
$b = UUID::fromBinary("\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2");
$c = UUID::fromBinary("\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3");

var_dump($a <= $b, $b <= $c, $a <= $c);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
