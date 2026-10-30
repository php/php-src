--TEST--
GH-18139 (Memory leak when overriding some settings via readline_info())
--EXTENSIONS--
readline
--FILE--
<?php

var_dump(readline_info('readline_name', 'first'));
var_dump(readline_info('readline_name', 'second'));
var_dump(readline_info('line_buffer', 'third'));
var_dump(readline_info('line_buffer', 'fourth'));

?>
--EXPECTF--
string(%d) "%S"
string(5) "first"
string(%d) "%S"
string(5) "third"
