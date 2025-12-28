--TEST--
Bug #64239 (debug_print_backtrace() changed behavior)
--FILE--
<?php
class A {
    use T2 { t2method as Bmethod; }
}

class C extends A {
    public static function Bmethod() {
        debug_print_backtrace(DEBUG_BACKTRACE_IGNORE_ARGS, 1);
    }
}

trait T2 {
    public static function t2method() {
        debug_print_backtrace(DEBUG_BACKTRACE_IGNORE_ARGS, 1);
    }
}

A::Bmethod();
A::t2method();

C::Bmethod();
C::t2method();
?>
--EXPECTF--
#0 %s(%d): A::Bmethod()
#0 %s(%d): A::t2method()
#0 %s(%d): C::Bmethod()
#0 %s(%d): A::t2method()
