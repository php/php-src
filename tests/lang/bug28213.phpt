--TEST--
Bug #28213 (crash in debug_print_backtrace in static methods)
--FILE--
<?php
class FooBar { static function error() { debug_print_backtrace(); } }
set_error_handler(array('FooBar', 'error'));
include('foobar.php');
?>
--EXPECTF--
#0 %s(%d): FooBar::error(2, 'include(foobar....', '%s', 4)
#1 %s(%d): include()
#0 %s(%d): FooBar::error(2, 'include(): Fail...', '%s', 4)
#1 %s(%d): include()
