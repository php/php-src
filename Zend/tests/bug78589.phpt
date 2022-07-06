--TEST--
Bug #78589: Memory leak with GC + __destruct()
--FILE--
<?php

class Test {
    public function __destruct() {}
}

$test = new Test;
$test->foo = [&$test->foo];
$ary = [&$ary, $test];
unset($ary, $test);
gc_collect_cycles();

?>
===DONE===
--EXPECT--
===DONE===
