--TEST--
Random: Randomizer: getFloat(): Opposite signs are handled correctly
--FILE--
<?php

use Random\Engine;
use Random\Randomizer;

final class TestEngine implements Engine
{
    private array $values = [
        "\x63\xe8\x58\x79\x3e\xf6\x38\x00",
        "\x64\xe8\x58\x79\x3e\xf6\x38\x00",
        "\x65\xe8\x58\x79\x3e\xf6\x38\x00",
        "\x66\xe8\x58\x79\x3e\xf6\x38\x00",
        "\x67\xe8\x58\x79\x3e\xf6\x38\x00",
    ];

    public function generate(): string
    {
        return \array_shift($this->values);
    }
}

$r = new Randomizer(new TestEngine());

$min = -1;
$max = 1;
printf("%.17g\n", $min);
printf("%.17g\n\n", $max);

$prev = null;
for ($i = 0; $i < 5; $i++) {
    $float = $r->getFloat($min, $max);
    printf("%.17f", $float);
    if ($prev !== null) {
        printf(" (%+.17g)", ($float - $prev));
    }
    printf("\n");

    $prev = $float;
}
?>
--EXPECT--
-1
1

-0.78005908680576086
-0.78005908680576097 (-1.1102230246251565e-16)
-0.78005908680576108 (-1.1102230246251565e-16)
-0.78005908680576119 (-1.1102230246251565e-16)
-0.78005908680576130 (-1.1102230246251565e-16)
