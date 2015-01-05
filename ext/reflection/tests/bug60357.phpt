--TEST--
Bug #60357 (__toString() method triggers E_NOTICE "Array to string conversion")
--FILE--
<?php
function foo( array $x = array( 'a', 'b' ) ) {}
$r = new ReflectionParameter( 'foo', 0 );
echo $r->__toString();
?>
--EXPECTF--
Parameter #0 [ <optional> array $x = Array ]
