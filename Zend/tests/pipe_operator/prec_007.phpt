--TEST--
Pipe precedence 007
--FILE--
<?php

$value = 42;
$value
    |> (fn ($x) => $x ?? throw new Exception('Value may not be null'))
    |> (fn ($x) => var_dump($x));

$value = null;
$value
    |> (fn ($x) => $x ?? throw new Exception('Value may not be null'))
    |> (fn ($x) => var_dump($x));

?>
--EXPECTF--
int(42)

Fatal error: Uncaught Exception: Value may not be null in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(NULL)
#1 {main}
  thrown in %s on line %d
