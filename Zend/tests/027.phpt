--TEST--
Testing dynamic calls using variable variables with curly syntax
--FILE--
<?php

$a = 'b';
$b = 'c';
$c = 'strtoupper';

var_dump(${${$a}}('foo') == 'FOO');

$a = 'b';
$b = 'c';
${"1"} = 'strtoupper';
$strtoupper = 'strtolower';

var_dump(${${++$a}}('FOO') == 'foo');

?>
--EXPECT--
bool(true)
bool(true)
