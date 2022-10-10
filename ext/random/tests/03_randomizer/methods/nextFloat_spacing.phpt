--TEST--
Random: Randomizer: nextFloat(): Return values are evenly spaced.
--FILE--
<?php

use Random\Engine;
use Random\Randomizer;

final class StaticEngine implements Engine
{
    public function __construct(private string $value)
    {
    }

    public function generate(): string
    {
        return $this->value;
    }
}

$zero = new Randomizer(new StaticEngine("\x00\x00\x00\x00\x00\x00\x00\x00"));
$one = new Randomizer(new StaticEngine("\x00\x08\x00\x00\x00\x00\x00\x00"));
$two = new Randomizer(new StaticEngine("\x00\x10\x00\x00\x00\x00\x00\x00"));

$max_minus_two = new Randomizer(new StaticEngine("\x00\xe8\xff\xff\xff\xff\xff\xff"));
$max_minus_one = new Randomizer(new StaticEngine("\x00\xf0\xff\xff\xff\xff\xff\xff"));
$max = new Randomizer(new StaticEngine("\x00\xf8\xff\xff\xff\xff\xff\xff"));

var_dump($one->nextFloat() - $one->nextFloat() === $zero->nextFloat());
var_dump($two->nextFloat() - $one->nextFloat() === $one->nextFloat());
var_dump($max->nextFloat() - $max_minus_one->nextFloat() === $one->nextFloat());
var_dump($max_minus_one->nextFloat() - $max_minus_two->nextFloat() === $one->nextFloat());
var_dump($max->nextFloat() - $max_minus_two->nextFloat() === $two->nextFloat());

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
