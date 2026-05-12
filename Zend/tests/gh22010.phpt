--TEST--
GH-22010: Exception thrown in destructor during shutdown
--FILE--
<?php

class A
{
    public function __destruct()
    {
        throw new Exception(__METHOD__);
    }
}

$a = new A;

?>
--EXPECTF--
Fatal error: Uncaught Exception: A::__destruct in %s:%d
Stack trace:
#0 [internal function]: A->__destruct()
#1 {main}
  thrown in %s on line %d
