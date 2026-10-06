--TEST--
GH-17154 readline_write_history()/readline_read_history(): memory leak
--EXTENSIONS--my
readline
--FILE--
<?php
readline_info('line_buffer', 'val');
readline_write_history("myhistory");
var_dump(readline_read_history("myhistory"));
?>
--EXPECTF--
bool(true)
