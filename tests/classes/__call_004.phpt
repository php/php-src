--TEST--
When __call() is invoked via ::, ensure current scope's __call() is favoured over the specified class's  __call().
--FILE--
<?php
class A {
    function __call($strMethod, $arrArgs) {
        echo "In " . __METHOD__ . "($strMethod, array(" . implode(',',$arrArgs) . "))\n";
        var_dump($this);
    }
}

class B extends A {
    function __call($strMethod, $arrArgs) {
        echo "In " . __METHOD__ . "($strMethod, array(" . implode(',',$arrArgs) . "))\n";
        var_dump($this);
    }

    function test() {
        A::test1(1,'a');
        B::test2(1,'a');
        self::test3(1,'a');
        parent::test4(1,'a');
    }
}

$b = new B();
$b->test();
?>
--EXPECT--
In B::__call(test1, array(1,a))
object(B)#1 (0) {
}
In B::__call(test2, array(1,a))
object(B)#1 (0) {
}
In B::__call(test3, array(1,a))
object(B)#1 (0) {
}
In B::__call(test4, array(1,a))
object(B)#1 (0) {
}
