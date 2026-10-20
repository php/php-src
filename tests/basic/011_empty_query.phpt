--TEST--
Testing $argc and $argv handling (GET empty)
--SKIPIF--
<?php
    if(substr(PHP_OS, 0, 3) == 'WIN') die("skip on windows: --INI-- is ignored due to 4b9cd27ff5c0177dcb160caeae1ea79e761ada58");
?>
--INI--
register_argc_argv=1
--CGI--
--FILE--
<?php

var_dump($_SERVER['argc'], $_SERVER['argv']);

?>
--EXPECTF--
Deprecated: Deriving $_SERVER['argv'] from the query string is deprecated. Configure register_argc_argv=0 to turn this message off in %s on line %d
int(0)
array(0) {
}
