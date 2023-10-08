--TEST--
GH-1093: Add separate static property through trait if parent already declares it
--FILE--
<?php
trait Foo {
    static $test;

    public static function getFooSelf() {
        return self::$test;
    }

    public static function getFooStatic() {
        return static::$test;
    }
}
trait Bar {
    public static function getBarSelf() {
        return self::$test;
    }

    public static function getBarStatic() {
        return static::$test;
    }
}

class A {
    use Foo;
    use Bar;

    public static function getASelf() {
        return self::$test;
    }

    public static function getAStatic() {
        return static::$test;
    }
}

class B extends A {
    use Foo;

    public static function getBSelf() {
        return self::$test;
    }

    public static function getBStatic() {
        return static::$test;
    }
}

A::$test = 'A';
B::$test = 'B';

echo 'A::$test: ' . A::$test . "\n";
echo 'A::getASelf(): ' . A::getASelf() . "\n";
echo 'A::getAStatic(): ' . A::getAStatic() . "\n";
echo 'A::getFooSelf(): ' . A::getFooSelf() . "\n";
echo 'A::getFooStatic(): ' . A::getFooStatic() . "\n";
echo 'B::$test: ' . B::$test . "\n";
echo 'B::getASelf(): ' . B::getASelf() . "\n";
echo 'B::getAStatic(): ' . B::getAStatic() . "\n";
echo 'B::getBSelf(): ' . B::getBSelf() . "\n";
echo 'B::getBStatic(): ' . B::getBStatic() . "\n";
echo 'B::getFooSelf(): ' . B::getFooSelf() . "\n";
echo 'B::getFooStatic(): ' . B::getFooStatic() . "\n";
echo 'B::getBarSelf(): ' . B::getBarSelf() . "\n";
echo 'B::getBarStatic(): ' . B::getBarStatic() . "\n";
?>
--EXPECT--
A::$test: A
A::getASelf(): A
A::getAStatic(): A
A::getFooSelf(): A
A::getFooStatic(): A
B::$test: B
B::getASelf(): A
B::getAStatic(): B
B::getBSelf(): B
B::getBStatic(): B
B::getFooSelf(): B
B::getFooStatic(): B
B::getBarSelf(): A
B::getBarStatic(): B
