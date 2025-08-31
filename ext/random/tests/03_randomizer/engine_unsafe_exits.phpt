--TEST--
Random: Randomizer: Engines calling exit() are gracefully handled
--FILE--
<?php

use Random\Engine;
use Random\Randomizer;

final class ExitingEngine implements Engine
{
    public function generate(): string
    {
        exit("Exit\n");
    }
}

$randomizer = new Randomizer(new ExitingEngine());

var_dump($randomizer->getBytes(1));

?>
--EXPECT--
Exit
