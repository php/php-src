--TEST--
Type inference of compound assignment to typed properties in strict mode
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x400000
opcache.preload=
--EXTENSIONS--
opcache
--FILE--
<?php
declare(strict_types=1);

class C {
    public int|string $is = 0;
    public float $f = 1.0;
}

function known(C $o, float $x, int $y) {
    return [($o->is += $x), ($o->f |= $y)];
}

function unknown($o, int $y) {
    $r = ($o->f |= $y);
    return [is_float($r), $r];
}

try {
    known(new C, INF, 2);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump(unknown(new C, 2));
?>
--EXPECTF--
known:
     ; (lines=10, args=3, vars=3, tmps=%d, ssa_vars=12, no_loops)
     ; (after dfa pass)
     ; %s
     ; return  []
     ; #0.CV0($o) NOVAL [undef]
     ; #1.CV1($x) NOVAL [undef]
     ; #2.CV2($y) NOVAL [undef]
BB0:
     ; start exit lines=[0-9]
     ; level=0
0000 #3.CV0($o) [object (instanceof C)] = RECV 1
0001 #4.CV1($x) [double] = RECV 2
0002 #5.CV2($y) [long] RANGE[MIN..MAX] = RECV 3
0003 #7.T3 [] = ASSIGN_OBJ_OP (ADD) #3.CV0($o) [object (instanceof C)] -> #6.CV0($o) [object (instanceof C)] string("is")
0004 OP_DATA #4.CV1($x) [double]
0005 #8.T4 [] = INIT_ARRAY 2 (packed) #7.T3 [] NEXT
0006 #10.T5 [double] = ASSIGN_OBJ_OP (BW_OR) #6.CV0($o) [object (instanceof C)] -> #9.CV0($o) NOVAL [object (instanceof C)] string("f")
0007 OP_DATA #5.CV2($y) [long] RANGE[MIN..MAX]
0008 ADD_ARRAY_ELEMENT #10.T5 [double] NEXT #8.T4 [] -> #11.T4 []
0009 RETURN #11.T4 []

unknown:
     ; (lines=8, args=2, vars=3, tmps=%d, ssa_vars=11, no_loops)
     ; (after dfa pass)
     ; %s
     ; return  [[packed, hash] array [long] of [false, true, long, double]]
     ; #0.CV0($o) NOVAL [undef]
     ; #1.CV1($y) NOVAL [undef]
     ; #2.CV2($r) NOVAL [undef]
BB0:
     ; start exit lines=[0-7]
     ; level=0
0000 #3.CV0($o) [any] = RECV 1
0001 #4.CV1($y) [long] RANGE[MIN..MAX] = RECV 2
0002 #7.CV2($r) [long, double] = ASSIGN_OBJ_OP (BW_OR) #3.CV0($o) [any] -> #5.CV0($o) NOVAL [object] string("f")
0003 OP_DATA #4.CV1($y) [long] RANGE[MIN..MAX]
0004 #8.T5 [bool] = TYPE_CHECK (double) #7.CV2($r) [long, double]
0005 #9.T6 [[packed, hash] array [long] of [false, true]] = INIT_ARRAY 2 (packed) #8.T5 [bool] NEXT
0006 ADD_ARRAY_ELEMENT #7.CV2($r) [long, double] NEXT #9.T6 [[packed, hash] array [long] of [false, true]] -> #10.T6 [[packed, hash] array [long] of [false, true, long, double]]
0007 RETURN #10.T6 [[packed, hash] array [long] of [false, true, long, double]]
Cannot assign float to property C::$is of type string|int
array(2) {
  [0]=>
  bool(true)
  [1]=>
  float(3)
}
