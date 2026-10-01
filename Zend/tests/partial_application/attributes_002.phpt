--TEST--
PFA preserves #[SensitiveParameter]
--FILE--
<?php

function f($a, #[SensitiveParameter] $b, $c, #[SensitiveParameter] ...$d) {
    throw new Exception();
}

$f = f(1, ?, ?, ?, ...)('normal param', 3, 'reified variadic', 'variadic');

?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 %s(%d): f(1, Object(SensitiveParameterValue), 3, Object(SensitiveParameterValue), Object(SensitiveParameterValue))
#1 %s(%d): {closure:pfa:%s:7}(Object(SensitiveParameterValue), 3, Object(SensitiveParameterValue), Object(SensitiveParameterValue))
#2 {main}
  thrown in %s on line %d
