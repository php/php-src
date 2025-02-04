--TEST--
get_error_handler()
--FILE--
<?php

class C {
    function handle() {}
    static function handleStatic() {}
}

function foo() {}

set_error_handler("foo");
var_dump(get_error_handler());

set_error_handler(null);
var_dump(get_error_handler());

set_error_handler([C::class, 'handleStatic']);
var_dump(get_error_handler());

set_error_handler('C::handleStatic');
var_dump(get_error_handler());

set_error_handler([new C(), 'handle']);
var_dump(get_error_handler());

set_error_handler((new C())->handle(...));
var_dump(get_error_handler());

set_error_handler(function () {});
var_dump(get_error_handler());

?>
==DONE==
--EXPECTF--
string(3) "foo"
NULL
array(2) {
  [0]=>
  string(1) "C"
  [1]=>
  string(12) "handleStatic"
}
string(15) "C::handleStatic"
array(2) {
  [0]=>
  object(C)#%d (0) {
  }
  [1]=>
  string(6) "handle"
}
object(Closure)#%d (2) {
  ["function"]=>
  string(9) "C::handle"
  ["this"]=>
  object(C)#%d (0) {
  }
}
object(Closure)#%d (3) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(%d)
}
==DONE==
