--TEST--
Random: Randomizer: getFloat(): Extreme ranges are handled correctly
--FILE--
<?php

use Random\Engine;
use Random\Randomizer;

final class TestEngine implements Engine
{
    private array $values = [
        "\x64\xe8\x58\x79\x3e\xf6\x38\x00",
        "\x65\xe8\x58\x79\x3e\xf6\x38\x00",
        "\x00\x00\x00\x00\x00\x00\x00\x00",
    ];

    public function generate(): string
    {
        return \array_shift($this->values);
    }
}

$r = new Randomizer(new TestEngine());

$min = -1.6e308;
$max = 1.6e308;
printf("%.17g\n", $min);
printf("%.17g\n\n", $max);

printf("%.17g\n", $r->getFloat($min, $max));
printf("%.17g\n", $r->getFloat($min, $max));
printf("%.17g\n", $r->getFloat($min, $max));

?>
--EXPECT--
-1.6e+308
1.6e+308

-1.5999999999999998e+308
-1.6e+308
1.5999999999999998e+308
