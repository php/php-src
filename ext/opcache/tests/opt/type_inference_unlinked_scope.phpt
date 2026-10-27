--TEST--
Type inference resolves class hints naming the unlinked declaring class
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x400000
opcache.preload=
zend_test.observer.enabled=0
--EXTENSIONS--
opcache
--FILE--
<?php

if (true) {
    final class A {
        public int $x = 1;
        public function f(A $a) { return $a->x; }
    }
}

class B extends C {
    public int $y = 1;
    public function f(B $b) { return $b->y; }
}

class C {}

?>
--EXPECTF--

$_main:
     ; (lines=3, args=0, vars=0, tmps=0, ssa_vars=0, no_loops)
     ; (after dfa pass)
     ; %s:1-18
     ; return  [long] RANGE[1..1]
BB0:
     ; start exit lines=[0-2]
     ; level=0
0000 DECLARE_CLASS string("a")
0001 DECLARE_CLASS_DELAYED string("b") string("c")
0002 RETURN int(1)

A::f:
     ; (lines=3, args=1, vars=1, tmps=1, ssa_vars=3, no_loops)
     ; (after dfa pass)
     ; %s:6-6
     ; return  [long]
     ; #0.CV0($a) NOVAL [undef]
BB0:
     ; start exit lines=[0-2]
     ; level=0
0000 #1.CV0($a) [object (A)] = RECV 1
0001 #2.T1 [long] = FETCH_OBJ_R #1.CV0($a) [object (A)] string("x")
0002 RETURN #2.T1 [long]

B::f:
     ; (lines=3, args=1, vars=1, tmps=1, ssa_vars=3, no_loops)
     ; (after dfa pass)
     ; %s:12-12
     ; return  [long]
     ; #0.CV0($b) NOVAL [undef]
BB0:
     ; start exit lines=[0-2]
     ; level=0
0000 #1.CV0($b) [object (instanceof B)] = RECV 1
0001 #2.T1 [long] = FETCH_OBJ_R #1.CV0($b) [object (instanceof B)] string("y")
0002 RETURN #2.T1 [long]
