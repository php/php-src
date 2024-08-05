--TEST--
GH-xxx 004: Crash during GC of suspended generator delegate
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
        gc_collect_cycles();
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

Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 %s(%d): It->getIterator()
#1 [internal function]: f()
#2 %s(%d): Generator->next()
#3 {main}
  thrown in %s on line %d
