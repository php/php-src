--TEST--
ZE2 Late Static Binding with exceptions 
--FILE--
<?php
function foo() {
    B::throwException();
}
class C {
    public static function bla() {
        B::throwException();
    }
    public static function getException() {
        return new Exception();
         
    }
}
class A {

    public static function throwException_after() {
        C::bla();
    }
    public static function throwException() {
        throw C::getException();
    }
    public static function test() {
        static::who();
    }
    public static function who() {
        echo "A\n";
    }

    public static function mycatch() {
        try {
            static::who();
            B::throwException_after();
        } catch(Exception $e) {
            static::who();
            A::test();
            static::who();
            B::test();
            static::who();

            self::simpleCatch();
            static::who();
        }
    }

    public static function simpleCatch() {
        try {
            static::who();
            throw new Exception();
        } catch (Exception $e) {
            static::who();
        }
    }
}

class B extends A {
    public static function who() {
        echo "B\n";
    }

}

echo "via A:\n";
A::myCatch();
echo "via B:\n";
B::myCatch();
?>
==DONE==
--EXPECTF--
via A:
A
A
A
A
B
A
A
A
A
via B:
B
B
A
B
B
B
B
B
B
==DONE==
