--TEST--
PFA in constexpr: binding a by-reference parameter
--FILE--
<?php

function byRef($a, &$b) {}
function byRefVariadic(&...$args) {}

class C {
    public static function staticByRef($a, &$b) {}
}

function f1($x = byRef(new stdClass, new stdClass, ...)) {}
function f2($x = byRef(b: new stdClass, a: ?)) {}
function f3($x = byRefVariadic(new stdClass, ...)) {}
function f4($x = byRefVariadic(extra: new stdClass, ...)) {}
function f5($x = C::staticByRef(new stdClass, new stdClass, ...)) {}

foreach (['f1', 'f2', 'f3', 'f4', 'f5'] as $f) {
    try {
        $f();
    } catch (Error $e) {
        echo get_class($e), ": ", $e->getMessage(), "\n";
    }
}

function f6($x = byRef(new stdClass, ?)) {
    return $x;
}

$partial = f6();
$var = 1;
var_dump($partial instanceof Closure);
$partial($var);

?>
--EXPECT--
Error: byRef(): Argument #2 ($b) could not be passed by reference
Error: byRef(): Argument #2 ($b) could not be passed by reference
Error: byRefVariadic(): Argument #1 could not be passed by reference
Error: byRefVariadic(): Argument #1 could not be passed by reference
Error: C::staticByRef(): Argument #2 ($b) could not be passed by reference
bool(true)
