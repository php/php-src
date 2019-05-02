--TEST--
Arrow function $this binding
--FILE--
<?php

class Test {
    public function method() {
        // It would be okay if this is NULL, but the rest should work
        $fn = fn() => 42;
        $r = new ReflectionFunction($fn);
        var_dump($r->getClosureThis());

        $fn = fn() => $this;
        var_dump($fn());

        $fn = fn() => Test::method2();
        $fn();

        $fn = fn() => call_user_func('Test::method2');
        $fn();

        $thisName = "this";
        $fn = fn() => $$thisName;
        var_dump($fn());

        $fn = fn() => self::class;
        var_dump($fn());

        // static can be used to unbind $this
        $fn = static fn() => isset($this);
        var_dump($fn());
    }

    public function method2() {
        var_dump($this);
    }
}

(new Test)->method();

?>
--EXPECT--
object(Test)#1 (0) {
}
object(Test)#1 (0) {
}
object(Test)#1 (0) {
}
object(Test)#1 (0) {
}
object(Test)#1 (0) {
}
string(4) "Test"
bool(false)
