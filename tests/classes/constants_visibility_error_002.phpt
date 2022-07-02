--TEST--
Class protected constant visibility error
--FILE--
<?php
class A {
    protected const protectedConst = 'protectedConst';
}

var_dump(A::protectedConst);

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot access protected constant A::protectedConst in %s:6
Stack trace:
#0 {main}
  thrown in %s on line 6
