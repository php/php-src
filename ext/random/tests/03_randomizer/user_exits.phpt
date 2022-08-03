--TEST--
Random: Randomizer: User: Engine exits
--FILE--
<?php

$randomizer = (new \Random\Randomizer(
    new class () implements \Random\Engine {
        public function generate(): string
        {
            exit("Exit\n");
        }
    }
));

var_dump($randomizer->getBytes(1));

?>
--EXPECT--
Exit
