--TEST--
Testing $argc and $argv handling (GET, register_argc_argv=0)
--SKIPIF--
<?php
    if(substr(PHP_OS, 0, 3) == 'WIN') die("skip on windows: --INI-- is ignored due to 4b9cd27ff5c0177dcb160caeae1ea79e761ada58");
?>
--INI--
register_argc_argv=0
--GET--
ab+cd+ef+123+test
--FILE--
<?php

var_dump($_SERVER['argc'], $_SERVER['argv']);

?>
--EXPECTF--
Warning: Undefined array key "argc" in %s on line %d

Warning: Undefined array key "argv" in %s on line %d
NULL
NULL
