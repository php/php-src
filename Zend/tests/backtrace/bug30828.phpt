--TEST--
Bug #30828 (debug_backtrace() reports incorrect class in overridden methods)
--FILE--
<?php
class A {
    function __construct() {
        debug_print_backtrace();
        $bt = debug_backtrace();
        foreach ($bt as $t) {
            print $t['class'].$t['type'].$t['function']."\n";
        }
    }

    function foo() {
        debug_print_backtrace();
        $bt = debug_backtrace();
        foreach ($bt as $t) {
                        print $t['class'].$t['type'].$t['function']."\n";
        }
    }

    static function bar() {
        debug_print_backtrace();
        $bt = debug_backtrace();
        foreach ($bt as $t) {
            print $t['class'].$t['type'].$t['function']."\n";
        }
    }
}

class B extends A {
    function __construct() {
        parent::__construct();
    }

    function foo() {
        parent::foo();
    }

    static function bar() {
        parent::bar();
    }
}

$b = new B();
$b->foo();
B::bar();
?>
--EXPECTF--
#0 %sbug30828.php(30): A->__construct()
#1 %sbug30828.php(42): B->__construct()
A->__construct
B->__construct
#0 %sbug30828.php(34): A->foo()
#1 %sbug30828.php(43): B->foo()
A->foo
B->foo
#0 %sbug30828.php(38): A::bar()
#1 %sbug30828.php(44): B::bar()
A::bar
B::bar
