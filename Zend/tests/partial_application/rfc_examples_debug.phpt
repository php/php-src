--TEST--
PFA RFC examples: "Debug output" section
--FILE--
<?php

function f($a, #[SensitiveParameter] $b, $c) {
    g();
}

function g() {
    throw new Exception();
}

$f = f(?, ?, 3);
$f(1, 2);

?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 %s(%d): g()
#1 %s(%d): f(1, Object(SensitiveParameterValue), 3)
#2 %s(%d): {closure:pfa:%s}(1, Object(SensitiveParameterValue))
#3 {main}
  thrown in %s on line %d
