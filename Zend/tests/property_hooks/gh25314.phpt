--TEST--
GH-25314 (Assertion in ZEND_DO_FCALL: reference returned by &__get() reaches property hook TMP result)
--CREDITS--
YuanchengJiang
--FILE--
<?php

class P {
    public int $d = 1;
    function &__get($name) {}
}
class C extends P {
    public int $d = 2 { get => parent::$d::get(); }
}
class GC extends C {
    public int $d { get => parent::$d::get(); }
}

var_dump((new GC)->d);

?>
--EXPECTF--
Notice: Only variable references should be returned by reference in %s on line %d

Fatal error: Uncaught TypeError: Value of type null returned from GC::__get() must be compatible with unset property GC::$d of type int in %s:%d
Stack trace:
#0 %s(%d): P->$d::get()
#1 %s(%d): C->$d::get()
#2 %s(%d): GC->$d::get()
#3 {main}
  thrown in %s on line %d
