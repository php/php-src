--TEST--
approx equality
--FILE--
<?php

// Number compares
var_dump(2 ~= 1); // false
var_dump(1.4 ~= 1); // true
var_dump(-1.4 ~= -1); // true
var_dump(-1.5 ~= -1.8); // true
var_dump(random_int(1, 1) ~= 1.1); // true

// Array compares
var_dump([1, 2, 3] ~= [2, 3, 4]); // true
var_dump([1, 2, 3] ~= [2, 3, 4, 5]); // false

// String / string compares
var_dump("This is a tpyo" ~= "This is a typo"); // true: No more typos
var_dump("something" ~= "different"); // false: clearly completely different
var_dump("Wtf bro" ~= "Wtf sis"); // true: Abolish concept of gender

// String / different type compares
var_dump(-1.5 ~= "-1.a"); // true
var_dump(-1.5 ~= "-1.aaaaaaa"); // false
var_dump(NULL ~= "blablabla"); // false

// AST dump test
assert(function() { return 1 ~= 2; } && false);

?>
--EXPECTF--
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)

Fatal error: Uncaught AssertionError: assert(function () {
    return 1 ~= 2;
} && false) in %s:%d
Stack trace:
#0 %s(%d): assert(false, 'assert(function...')
#1 {main}
  thrown in %s on line %d
