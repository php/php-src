--TEST--
Bug #61998 (Using traits with method aliases appears to result in crash during execution)
--FILE--
<?php
class Foo {
    use T1 {
       func as newFunc;
    }

    public function func() {
        echo "From Foo\n";
    }
}

trait T1 { 
    public function func() {
        echo "From T1\n";
    }
}

class Bar {
    public function func() {
        echo "From Bar\n";
    }
    public function func2() {
        echo "From Bar\n";
    }
    public function func3() {
        echo "From Bar\n";
    }
    use T1 {
        func as newFunc;
        func as func2;
    }
    use T2 {
        func2 as newFunc2;
        func2 as newFunc3;
        func2 as func3;
    }
}

trait T2 { 
    public function func2() {
        echo "From T2\n";
    }
}

$f = new Foo();

$f->newFunc(); //from T1
$f->func(); //from Foo

$b = new Bar();
$b->newFunc(); //from T1
$b->func(); //from Bar
$b->func2(); //from Bar
$b->newFunc2(); //from T2
$b->newFunc3(); //from T2
$b->func3(); //from Bar
--EXPECTF--
From T1
From Foo
From T1
From Bar
From Bar
From T2
From T2
From Bar
