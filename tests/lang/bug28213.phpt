--TEST--
Bug #28213 (crash in debug_print_backtrace in static methods)
--FILE--
<?php
class FooBar { static function error() { debug_print_backtrace(); } }
set_error_handler(array('FooBar', 'error'));
include('foobar.php');
?>
--EXPECTREGEX--
.*#1\s*include.*
