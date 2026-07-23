--TEST--
PFA RFC examples: "Constant expressions" section
--FILE--
<?php

class Foo
{
    public function __construct(
        private \Closure $callback = bar(1, 2, ?),
    ) {}
}

class Example
{
    public const BASE = 10;

    private Closure $arrayToInt = array_map(intval(?, self::BASE), ?);
}

?>
==DONE==
--EXPECTF--
==DONE==
