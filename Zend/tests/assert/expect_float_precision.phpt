--TEST--
assert() message preserves float literals regardless of precision
--INI--
zend.assertions=1
precision=14
--FILE--
<?php
foreach ([
    fn() => assert(0.30000000000000004 === 0.3),
    fn() => assert(123456789012345678.0 === 0.0),
    fn() => assert(0.1 === 1.5e-300),
    fn() => assert(1.0 === 2.0),
] as $fn) {
    try {
        $fn();
    } catch (AssertionError $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
AssertionError: assert(0.30000000000000004 === 0.3)
AssertionError: assert(1.2345678901234568E+17 === 0.0)
AssertionError: assert(0.1 === 1.5E-300)
AssertionError: assert(1.0 === 2.0)
