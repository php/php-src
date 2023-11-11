--TEST--
Random: Randomizer: Engines throwing an exception are gracefully handled
--FILE--
<?php

use Random\Engine;
use Random\Randomizer;

final class ThrowingEngine implements Engine
{
    public function generate(): string
    {
        throw new Exception('Error');
    }
} 

$randomizer = new Randomizer(new ThrowingEngine());

var_dump($randomizer->getBytes(1));

?>
--EXPECTF--
Fatal error: Uncaught Exception: Error in %s:%d
Stack trace:
#0 [internal function]: ThrowingEngine->generate()
#1 %s(%d): Random\Randomizer->getBytes(1)
#2 {main}
  thrown in %s on line %d
