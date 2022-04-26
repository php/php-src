--TEST--
Property types in inference
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x200000
opcache.preload=
--EXTENSIONS--
opcache
--FILE--
<?php

class Test {
    public bool $public;
    protected int $protected;
    private float $private;

    public function inTest() {
        var_dump($this->public, $this->protected, $this->private);
    }

    public function inTestWithTest2(Test2 $test2) {
        var_dump($test2->public, $test2->protected, $test2->private);
    }
}

class Test2 extends Test {
    private array $private;

    public function inTest2() {
        var_dump($this->public, $this->protected, $this->private);
    }
}

function noScope(Test $test) {
    var_dump($test->public, $test->protected, $test->private);
}

?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0, ssa_vars=0, no_loops)
     ; (before dfa pass)
     ; %s
     ; return  [long] RANGE[1..1]
BB0:
     ; start exit lines=[0-0]
     ; level=0
0000 RETURN int(1)

noScope:
     ; (lines=10, args=1, vars=1, tmps=4, ssa_vars=5, no_loops)
     ; (before dfa pass)
     ; %s
     ; return  [null] RANGE[0..0]
     ; #0.CV0($test) NOVAL [undef]
BB0:
     ; start exit lines=[0-9]
     ; level=0
0000 #1.CV0($test) [object (instanceof Test)] = RECV 1
0001 INIT_FCALL 3 %d string("var_dump")
0002 #2.T1 [bool] = FETCH_OBJ_R #1.CV0($test) [object (instanceof Test)] string("public")
0003 SEND_VAL #2.T1 [bool] 1
0004 #3.T2 [any] = FETCH_OBJ_R #1.CV0($test) [object (instanceof Test)] string("protected")
0005 SEND_VAL #3.T2 [any] 2
0006 #4.T3 [any] = FETCH_OBJ_R #1.CV0($test) [object (instanceof Test)] string("private")
0007 SEND_VAL #4.T3 [any] 3
0008 DO_ICALL
0009 RETURN null

Test::inTest:
     ; (lines=9, args=0, vars=0, tmps=4, ssa_vars=3, no_loops)
     ; (before dfa pass)
     ; %s
     ; return  [null] RANGE[0..0]
BB0:
     ; start exit lines=[0-8]
     ; level=0
0000 INIT_FCALL 3 %d string("var_dump")
0001 #0.T0 [bool] = FETCH_OBJ_R THIS string("public")
0002 SEND_VAL #0.T0 [bool] 1
0003 #1.T1 [long] = FETCH_OBJ_R THIS string("protected")
0004 SEND_VAL #1.T1 [long] 2
0005 #2.T2 [double] = FETCH_OBJ_R THIS string("private")
0006 SEND_VAL #2.T2 [double] 3
0007 DO_ICALL
0008 RETURN null

Test::inTestWithTest2:
     ; (lines=10, args=1, vars=1, tmps=4, ssa_vars=5, no_loops)
     ; (before dfa pass)
     ; %s
     ; return  [null] RANGE[0..0]
     ; #0.CV0($test2) NOVAL [undef]
BB0:
     ; start exit lines=[0-9]
     ; level=0
0000 #1.CV0($test2) [object (instanceof Test2)] = RECV 1
0001 INIT_FCALL 3 %d string("var_dump")
0002 #2.T1 [bool] = FETCH_OBJ_R #1.CV0($test2) [object (instanceof Test2)] string("public")
0003 SEND_VAL #2.T1 [bool] 1
0004 #3.T2 [long] = FETCH_OBJ_R #1.CV0($test2) [object (instanceof Test2)] string("protected")
0005 SEND_VAL #3.T2 [long] 2
0006 #4.T3 [double] = FETCH_OBJ_R #1.CV0($test2) [object (instanceof Test2)] string("private")
0007 SEND_VAL #4.T3 [double] 3
0008 DO_ICALL
0009 RETURN null

Test2::inTest2:
     ; (lines=9, args=0, vars=0, tmps=4, ssa_vars=3, no_loops)
     ; (before dfa pass)
     ; %s
     ; return  [null] RANGE[0..0]
BB0:
     ; start exit lines=[0-8]
     ; level=0
0000 INIT_FCALL 3 %d string("var_dump")
0001 #0.T0 [bool] = FETCH_OBJ_R THIS string("public")
0002 SEND_VAL #0.T0 [bool] 1
0003 #1.T1 [long] = FETCH_OBJ_R THIS string("protected")
0004 SEND_VAL #1.T1 [long] 2
0005 #2.T2 [array of [any, ref]] = FETCH_OBJ_R THIS string("private")
0006 SEND_VAL #2.T2 [array of [any, ref]] 3
0007 DO_ICALL
0008 RETURN null
