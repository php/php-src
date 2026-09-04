--TEST--
GH-20221 (min()/max() tie-break inconsistently on signed zero via the 2-arg frameless fast path)
--FILE--
<?php

$negZero = -0.0;

// Direct calls with exactly two positional arguments are compiled to the
// frameless min()/max() opcodes, which used to break ties on -0.0 vs 0
// differently from every other min()/max() code path (and from each other).
var_dump(min($negZero, 0));
var_dump(max($negZero, 0));
var_dump(min(0, $negZero));
var_dump(max(0, $negZero));

// The variadic (3+ args) and array forms already resolved ties in favor of
// the first-seen operand; the frameless 2-arg results above must now agree.
var_dump(min($negZero, 0, 0));
var_dump(max($negZero, 0, 0));
var_dump(min([$negZero, 0]));
var_dump(max([$negZero, 0]));

?>
--EXPECT--
float(-0)
float(-0)
int(0)
int(0)
float(-0)
float(-0)
float(-0)
float(-0)
