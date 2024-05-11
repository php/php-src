--TEST--
GH-14003: Missing handling of CALLABLE_CONVERT in cleanup_unfinished_calls()
--FILE--
<?php

function foo(string $key): string {
    throw new \Exception('Test');
}

array_filter(
    array_combine(
        ['a'],
        array_map(foo(...), ['a']),
    ),
);

?>
--EXPECTF--
Fatal error: Uncaught Exception: Test in %s:%d
Stack trace:
#0 [internal function]: foo('a')
#1 %s(%d): array_map(Object(Closure), Array)
#2 {main}
  thrown in %s on line %d
