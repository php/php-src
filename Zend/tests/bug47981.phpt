--TEST--
Bug #47981 (error handler not called regardless)
--INI--
error_reporting=0
--FILE--
<?php
function errh($errno, $errstr) {
	var_dump($errstr);
}
set_error_handler("errh");

interface a{}
class b implements a { function f($a=1) {}}
class c extends b {function f() {}}
?>
--EXPECT--
string(60) "Declaration of c::f() should be compatible with b::f($a = 1)"
