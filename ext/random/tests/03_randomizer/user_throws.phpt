--TEST--
Random: Randomizer: User: Engine throws
--FILE--
<?php

$randomizer = (new \Random\Randomizer(
    new class () implements \Random\Engine {
        public function generate(): string
        {
            throw new Exception('Error');
        }
    }
));

var_dump($randomizer->getBytes(1));

?>
--EXPECTF--
Fatal error: Uncaught Exception: Error in %s:%d
Stack trace:
#0 [internal function]: Random\Engine@anonymous->generate()
#1 %s(%d): Random\Randomizer->getBytes(1)
#2 {main}

Next RuntimeException: Random number generation failed in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->getBytes(1)
#1 {main}
  thrown in %s on line %d
