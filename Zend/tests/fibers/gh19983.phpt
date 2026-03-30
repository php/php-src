--TEST--
GH-19983 (GC Assertion Failure with fibers, generators and destructors)
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) die("skip 64-bit only - fiber stacks exhaust 32-bit address space");
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--INI--
memory_limit=16M
--FILE--
<?php
class a
{
    function __destruct()
    {
        $gen = (function () {
            $from = (function () {
                $cv = [new a];
                Fiber::suspend();
            })();
            yield from $from;
        })();
        $fiber = new Fiber(function () use ($gen, &$fiber) {
            $gen->current();
        });
        $fiber->start();
    }
}
new a;
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%s
