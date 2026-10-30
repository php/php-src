--TEST--
readline_info(): using line_buffer before rl_line_buffer is initialised.
--EXTENSIONS--
readline
--SKIPIF--
<?php if (READLINE_LIB == "libedit") die("skip readline only");
if (getenv('SKIP_REPEAT')) die("skip readline has global state");
?>
--FILE--
<?php

$name = tempnam('/tmp', 'readline.tmp');

var_dump(readline_info('line_buffer'));
readline_info('line_buffer', 'abcdef');
var_dump(readline_info('line_buffer'));
readline_add_history('123');
readline_write_history($name);
readline_info('line_buffer', 'abcdefghijkl');
var_dump(readline_info('line_buffer'));

var_dump(file_get_contents($name));

unlink($name);
?>
--EXPECTF--
string(0) ""
string(6) "abcdef"
string(12) "abcdefghijkl"
string(4) "123
"
