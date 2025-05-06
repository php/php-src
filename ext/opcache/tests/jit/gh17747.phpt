--TEST--
GH-17747 (Exception on reading property in register-based FETCH_OBJ_R breaks JIT)
--EXTENSIONS--
opcache
--INI--
opcache.jit=function
--FILE--
<?php
class C {
    public int $a;
    public function test() {
        var_dump($this->a);
    }
}
$test = new C;
$test->test();
?>
--EXPECTF--
Fatal error: Uncaught Error: Typed property C::$a must not be accessed before initialization in %s:%d
Stack trace:
#0 %s(%d): C->test()
#1 {main}
  thrown in %s on line %d
