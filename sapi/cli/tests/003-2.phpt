--TEST--
defining INI options with -d (as 2nd arg)
--SKIPIF--
<?php
include "skipif.inc";
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die ("skip not for Windows");
}
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

var_dump(`"$php" -nd max_execution_time=111 -r 'var_dump(ini_get("max_execution_time"));'`);
var_dump(`"$php" -nd max_execution_time=500 -r 'var_dump(ini_get("max_execution_time"));'`);

?>
--EXPECT--
string(16) "string(3) "111"
"
string(16) "string(3) "500"
"
