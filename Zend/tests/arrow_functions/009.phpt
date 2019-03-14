--TEST--
Arrow function $this binding
--FILE--
<?php

class Test {
    public function method() {
        // It would be okay if this is NULL, but the rest should work
        $fn = () ==> 42;
        $r = new ReflectionFunction($fn);
        var_dump($r->getClosureThis());

        $fn = () ==> $this;
        var_dump($fn());

        $fn = () ==> Test::method2();
        $fn();

        $fn = () ==> call_user_func('Test::method2');
        $fn();

        $thisName = "this";
        $fn = () ==> $$thisName;
        var_dump($fn());

        $fn = () ==> self::class;
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
