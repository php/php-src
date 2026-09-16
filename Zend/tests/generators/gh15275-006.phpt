--TEST--
GH-15275 006: Crash during GC of suspended generator delegate
--FILE--
<?php

class It implements \IteratorAggregate
{
    public function getIterator(): \Generator
    {
        yield 'foo';
        echo "baz\n";
        throw new \Exception();
    }

    public function __destruct()
    {
        throw new \Exception();
    }
}

function f() {
    var_dump(new stdClass, yield from new It());
}

$gen = f();

var_dump($gen->current());
$gen->next();

gc_collect_cycles();

?>
==DONE==
--EXPECTF--
string(3) "foo"
baz

Fatal error: Uncaught Exception in %s:9
Stack trace:
#0 %s(19): It->getIterator()
#1 [internal function]: f()
#2 %s(25): Generator->next()
#3 {main}

Next Exception in %s:14
Stack trace:
#0 %s(19): It->__destruct()
#1 [internal function]: f()
#2 %s(25): Generator->next()
#3 {main}
  thrown in %s on line 14
