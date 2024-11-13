--TEST--
Late Static Binding static:: accesses protected / public static variables of child 
class when the variable is private in parent class
--FILE--
<?php
class A {
    private static $value = 'A';

    public static function out() {
        echo static::$value, PHP_EOL;
    }
}
class B extends A {
    protected static $value = 'B';
}
class C extends A {
    public static $value = 'C';
}
A::out();
B::out();
C::out();
?>
--EXPECT--
A
B
C
