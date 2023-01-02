--TEST--
Bug #47109 (Memory leak on $a->{"a"."b"} when $a is not an object)
--FILE--
<?php
$a->{"a"."b"};
?>
--EXPECTF--
Warning: Undefined variable $a (this will become an error in PHP 9.0) in %s on line %d

Warning: Attempt to read property "ab" on null in %s on line %d
