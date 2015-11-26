--TEST--
Bug #38600 (infinite loop in pcre with extended class)
--FILE--
<?php
$foo = 'bla bla bla';

var_dump(preg_match('/(?<!\w)(0x[\p{N}]+[lL]?|[\p{Nd}]+(e[\p{Nd}]*)?[lLdDfF]?)(?!\w)/', $foo, $m));
var_dump($m);

?>
--EXPECT--
int(0)
array(0) {
}
