--TEST--
GH-16812 readline_info(): UAF
--EXTENSIONS--
readline
--SKIPIF--
<?php
if (getenv('SKIP_REPEAT')) die("skip readline has global state");
?>
--FILE--
<?php
readline_write_history(NULL);
var_dump(readline_info('line_buffer', 'test'));
?>
--EXPECT--
string(0) ""
