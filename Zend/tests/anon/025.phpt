--TEST--
anonymous class cannot use variable twice
--FILE--
<?php
$glow = 10;

new class use($glow, $glow) {};
?>
--EXPECTF--
Fatal error: Cannot use property name glow twice in %s on line 4


