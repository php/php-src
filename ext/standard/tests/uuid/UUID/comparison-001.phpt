--TEST--
UUID comparison reflexivity
--DESCRIPTION--
𝑎 ≼ 𝑎

https://en.wikipedia.org/wiki/Reflexive_relation
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$a = UUID::fromBinary("\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1");

var_dump($a <= $a);

?>
--EXPECT--
bool(true)
