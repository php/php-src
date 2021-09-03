--TEST--
Bug #80299: ReflectionFunction->invokeArgs confused in arguments
--FILE--
<?php

$bar = new DateTime();
$args = [1, &$bar];

$function = function (int &$foo, DateTimeInterface &$bar) {};

(new ReflectionFunction($function))->invokeArgs($args);

?>
--EXPECTF--
Warning: {closure}(): Argument #1 ($foo) must be passed by reference, value given in %s on line %d
