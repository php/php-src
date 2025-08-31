<?php
function f1() {
}

if (isset($rt)) {
    function f2() {
    }
}

interface a {
    function foo();
    function bar();
}
interface b {
    function foo();
}

abstract class c {
    function bar() { }
}

class x extends c implements a, b {
    function foo() { }
}

trait T1 {
    #[A]
    public $prop;

    static function foo() {
        var_dump(__METHOD__);
    }
}
trait T2 {
    use T1;
    static function bar() {
        var_dump(__METHOD__);
    }
}
class Y {
    use T2;
}

class Z {
    public $foo;
    public a $bar;
}

class Z2 extends Z {}

class TraitAliasTest {
    use T1 {
        T1::foo as bar;
    }
}

// Create reference to a property declared in an internal parent class.
class MyException extends Exception {
    public function __construct($message) {
        $this->message =& $message;
    }
}

function get_anon() {
    return new class {};
}

if (!isset($rt)) {
    eval("class Foo {}");

    class Bar extends Foo {}

    eval("function f3() {} ");
}
