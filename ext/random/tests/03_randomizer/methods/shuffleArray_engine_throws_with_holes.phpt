--TEST--
Random: Randomizer: shuffleArray(): Engine throwing on an array with holes
--FILE--
<?php

use Random\Engine;
use Random\Randomizer;

final class ThrowingEngine implements Engine
{
    public function __construct(private int $calls) {}

    public function generate(): string
    {
        if ($this->calls-- <= 0) {
            throw new Exception('Error');
        }

        return "\x01\x00\x00\x00\x00\x00\x00\x00";
    }
}

foreach ([0, 1] as $calls) {
    $array = [new stdClass, new stdClass, new stdClass, new stdClass];
    unset($array[1]);

    try {
        (new Randomizer(new ThrowingEngine($calls)))->shuffleArray($array);
    } catch (Exception $e) {
        echo $e->getMessage(), PHP_EOL;
    }

    var_dump($array);
}

?>
--EXPECTF--
Error
array(3) {
  [0]=>
  object(stdClass)#%d (0) {
  }
  [2]=>
  object(stdClass)#%d (0) {
  }
  [3]=>
  object(stdClass)#%d (0) {
  }
}
Error
array(3) {
  [0]=>
  object(stdClass)#%d (0) {
  }
  [2]=>
  object(stdClass)#%d (0) {
  }
  [3]=>
  object(stdClass)#%d (0) {
  }
}
