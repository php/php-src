--TEST--
Test registering of functions
--PHPDBG--
R testfunc
testfunc 1 2 3
R var_dump
var_dump foo
q
--FILE--
<?php
function testfunc() {
    var_dump(func_get_args());
}
?>
--EXPECTF--
[Successful compilation of %s]
prompt> [Registered testfunc]
prompt> array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}

prompt> [Registered var_dump]
prompt> string(3) "foo"

prompt>
