--TEST--
ZE2 Late Static Binding stacking static calleds
--FILE--
<?php

class TestA {
    public static function test() {
        echo get_class(new static()) . "\n";
        TestB::test();
        echo get_class(new static()) . "\n";
        TestC::test();
        echo get_class(new static()) . "\n";
        TestBB::test();
        echo get_class(new static()) . "\n";
    }
}

class TestB {
    public static function test() {
        echo get_class(new static()) . "\n";
        TestC::test();
        echo get_class(new static()) . "\n";
    }
}

class TestC {
    public static function test() {
        echo get_class(new static()) . "\n";
    }
}

class TestBB extends TestB {
}

TestA::test();

?>
--EXPECT--
TestA
TestB
TestC
TestB
TestA
TestC
TestA
TestBB
TestC
TestBB
TestA
