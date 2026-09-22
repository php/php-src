--TEST--
GC 038: Garbage created by compound assignment operators (e.g. +=)
--INI--
zend.enable_gc = 1
--FILE--
<?php
function test_add() {
    $x = new stdClass;
    $x->x = $x;
    try {
        $x += 5;
    } catch (TypeError $e) { unset($x); }
    $n = gc_collect_cycles();
    echo "+=\t$n\n";
}
test_add();

function test_sub() {
    $x = new stdClass;
    $x->x = $x;
    try {
        $x -= 5;
    } catch (TypeError $e) { unset($x); }
    $n = gc_collect_cycles();
    echo "-=\t$n\n";
}
test_sub();

function test_mul() {
    $x = new stdClass;
    $x->x = $x;
    try {
        $x *= 5;
    } catch (TypeError $e) { unset($x); }
    $n = gc_collect_cycles();
    echo "*=\t$n\n";
}
test_mul();

function test_div() {
    $x = new stdClass;
    $x->x = $x;
    try {
        $x /= 5;
    } catch (TypeError $e) { unset($x); }
    $n = gc_collect_cycles();
    echo "/=\t$n\n";
}
test_div();

function test_mod() {
    $x = new stdClass;
    $x->x = $x;
    try {
        $x %= 5;
    } catch (TypeError $e) { unset($x); }
    $n = gc_collect_cycles();
    echo "%=\t$n\n";
}
test_mod();

function test_sl() {
    $x = new stdClass;
    $x->x = $x;
    try {
        $x <<= 5;
    } catch (TypeError $e) { unset($x); }
    $n = gc_collect_cycles();
    echo "<<=\t$n\n";
}
test_sl();

function test_sr() {
    $x = new stdClass;
    $x->x = $x;
    try {
        $x >>= 5;
    } catch (TypeError $e) { unset($x); }
    $n = gc_collect_cycles();
    echo ">>=\t$n\n";
}
test_sr();

function test_or() {
    $x = new stdClass;
    $x->x = $x;
    try {
        $x |= 5;
    } catch (TypeError $e) { unset($x); }
    $n = gc_collect_cycles();
    echo "|=\t$n\n";
}
test_or();

function test_and() {
    $x = new stdClass;
    $x->x = $x;
    try {
        $x &= 5;
    } catch (TypeError $e) { unset($x); }
    $n = gc_collect_cycles();
    echo "&=\t$n\n";
}
test_and();

function test_xor() {
    $x = new stdClass;
    $x->x = $x;
    try {
        $x ^= 5;
    } catch (TypeError $e) { unset($x); }
    $n = gc_collect_cycles();
    echo "^=\t$n\n";
}
test_xor();

function test_pow() {
    $x = new stdClass;
    $x->x = $x;
    try {
        $x **= 5;
    } catch (TypeError $e) { unset($x); }
    $n = gc_collect_cycles();
    echo "**=\t$n\n";
}
test_pow();

class Y {
    public $x;
    function __toString() {
        return "y";
    }
}
function test_concat() {
    $x = new Y;
    $x->x = $x;
    @$x .= "x";
    $n = gc_collect_cycles();
    echo ".=\t$n\n";
}
test_concat();
?>
--EXPECTF--
+=	1
-=	1
*=	1
/=	1
%=	1
<<=	1
>>=	1
|=	1
&=	1
^=	1
**=	1
.=	1
