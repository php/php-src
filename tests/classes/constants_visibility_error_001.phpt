--TEST--
Class private constant visibility error
--FILE--
<?php
class A {
    private const privateConst = 'privateConst';
}

var_dump(A::privateConst);

?>
--EXPECTF--
Fatal error: Uncaught Error: Private constant A::privateConst cannot be accessed from the global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line 6
