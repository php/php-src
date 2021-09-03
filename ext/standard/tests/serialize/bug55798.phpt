--TEST--
Bug #55798 (serialize followed by unserialize with numeric object prop. gives integer prop)
--FILE--
<?php

$a = new stdClass();
$a->{0} = 'X';
$a->{1} = 'Y';
var_dump(serialize($a));
var_dump($a->{0});
$b = unserialize(serialize($a));
var_dump(serialize($b));
var_dump($b->{0});
?>
--EXPECT--
string(51) "O:8:"stdClass":2:{s:1:"0";s:1:"X";s:1:"1";s:1:"Y";}"
string(1) "X"
string(51) "O:8:"stdClass":2:{s:1:"0";s:1:"X";s:1:"1";s:1:"Y";}"
string(1) "X"
