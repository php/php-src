--TEST--
GH-15275 003: Crash during GC of suspended generator delegate
--FILE--
<?php

class It implements \IteratorAggregate
{
    public function getIterator(): \Generator
    {
        yield 'foo';
        throw new \Exception();
        var_dump("not executed");
    }
}

function f() {
    try {
        var_dump(new stdClass, yield from new It());
    } finally {
        var_dump(__FUNCTION__);
    }
}

function g() {
    try {
        var_dump(new stdClass, yield from f());
    } finally {
        var_dump(__FUNCTION__);
    }
}

$gen = g();

var_dump($gen->current());
$gen->next();

?>
==DONE==
--EXPECTF--
string(3) "foo"
string(1) "f"
string(1) "g"

Fatal error: Uncaught Exception in %s:8
Stack trace:
#0 %s(15): It->getIterator()
#1 %s(23): f()
#2 [internal function]: g()
#3 %s(32): Generator->next()
#4 {main}
  thrown in %s on line 8
