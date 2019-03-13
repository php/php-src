--TEST--
Arrow function $this binding
--FILE--
<?php

class Test {
    public function method() {
        $fn = fn() => $this;
        var_dump($fn());

        $fn = fn() => 42;
        $r = new ReflectionFunction($fn);
        var_dump($r->getClosureThis());

        $fn = fn() => Test::method2();
        $fn();

        $fn = fn() => self::class;
        var_dump($fn());

        $thisName = "this";
        $fn = fn() => $$thisName;
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
NULL
object(Test)#1 (0) {
}
string(4) "Test"
object(Test)#1 (0) {
}
