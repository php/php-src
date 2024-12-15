--TEST--
readline_write_history(): Basic test
--EXTENSIONS--
readline
--SKIPIF--
<?php
if (READLINE_LIB != "libedit") die("skip libedit only");
if (getenv('SKIP_REPEAT')) die("skip readline has global state");
?>
--FILE--
<?php

$name = tempnam(sys_get_temp_dir(), 'readline.tmp');

readline_add_history('foo');
readline_add_history('');
readline_add_history(1);
readline_write_history($name);

echo(file_get_contents($name));

unlink($name);

?>
--EXPECTREGEX--
(_HiStOrY_V2_
)?foo
(
)?1
