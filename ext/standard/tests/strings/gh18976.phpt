--TEST--
GH-18976 (pack overflow with h/H format)
--INI--
memory_limit=-1
--FILE--
<?php
pack('h2147483647', 1);
pack('H2147483647', 1);
?>
--EXPECTF--

Warning: pack(): Type h: not enough characters in string in %s on line %d

Warning: pack(): Type H: not enough characters in string in %s on line %d
