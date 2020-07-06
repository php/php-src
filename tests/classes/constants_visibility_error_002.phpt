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
Fatal error: Uncaught Error: Protected constant A::protectedConst cannot be accessed from the global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line 6
