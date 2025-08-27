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
Deprecated: Deriving $_SERVER['argc'] and $_SERVER['argv'] from $_SERVER['QUERY_STRING'] is deprecated, configure register_argc_argv=0 to suppress this message and access the query parameters via $_SERVER['QUERY_STRING'] or $_GET in %s on line %d
int(0)
array(0) {
}
