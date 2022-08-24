--TEST--
Random: Randomizer: getBytes(): Returned bytes are consistently expanded
--FILE--
<?php

use Random\Engine;
use Random\Randomizer;

final class TestEngine implements Engine
{
    private int $count = 0;

    public function generate(): string
    {
        return match ($this->count++) {
            0 => 'H',
            1 => 'e',
            2 => 'll',
            3 => 'o',
            4 => 'abcdefghijklmnopqrstuvwxyz',
            5 => 'ABCDEFGHIJKLMNOPQRSTUVWXYZ',
            6 => 'success',
            default => throw new \Exception('Unhandled'),
        };
    }
}

$randomizer = new Randomizer(new TestEngine());

// 0-3: "Hello" - Insufficient bytes are concatenated.
var_dump($randomizer->getBytes(5));

// 4-5: "abcdefghABC" - Returned values are truncated to 64-bits for technical reasons, thus dropping i-z.
var_dump($randomizer->getBytes(11));

// 6: "success"
var_dump($randomizer->getBytes(7));

?>
--EXPECT--
string(5) "Hello"
string(11) "abcdefghABC"
string(7) "success"
