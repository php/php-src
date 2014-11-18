--TEST--
Bug #67985 - Last used array index not copied to new array at assignment
--FILE--
<?php

$a = ['zero', 'one', 'two'];
unset($a[2]);
$b = $a;
$a[] = 'three';
$b[] = 'three';

var_dump($a === $b);

?>
--EXPECT--
bool(true)
