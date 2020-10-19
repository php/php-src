--TEST--
Short methods.
--FILE--
<?php

class Test {

    public function __construct(private int $b) {}

    public function out(array $a) => foreach ($a as $v) print $v;
}

$t = new Test(1);

print $t->out([1, 2, 3]) . PHP_EOL;

?>
--EXPECTF--
Parse error: syntax error, unexpected token "foreach" in %s on line %d
