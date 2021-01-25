--TEST--
Test function: rng_bytes() generate strings.
--FILE--
<?php

class StringGenerator implements \RNG\RNGInterface
{
    private int $cnt = 0;

    public function next(): int
    {
        $method = "gen{$this->cnt}";
        $this->cnt++;
        return $this->$method();
    }

    public function next64(): int
    {
        return $this->next();
    }

    private function gen0(): int
    {
        $t = ord('l');
        $t = $t << 8 | ord('l');
        $t = $t << 8 | ord('e');
        $t = $t << 8 | ord('H');
        return $t;
    }

    private function gen1(): int
    {
        $t = ord('o');
        $t = $t << 8 | ord("w");
        $t = $t << 8 | ord(' ');
        $t = $t << 8 | ord('o');
        return $t;
    }

    private function gen2(): int
    {
        $t = ord('.');
        $t = $t << 8 | ord('d');
        $t = $t << 8 | ord('l');
        $t = $t << 8 | ord('r');
        return $t;
    }

    private function gen3(): int
    {
        return 0;
    }
}

$result = \rng_bytes(new StringGenerator(), 12);
if ($result !== 'Hello world.') {
    die("NG, rng_bytes() invalid result: ${result}");
}

$result = \rng_bytes(new StringGenerator(), 4);
if ($result !== 'Hell') {
    die("NG, rng_bytes() invalid result: ${result}");
}

$result = \rng_bytes(new StringGenerator(), 2);
if ($result !== 'He') {
    die("NG, rng_bytes() invalid result: ${result}");
}

die('OK, rng_bytes() works correctly.');
?>
--EXPECT--
OK, rng_bytes() works correctly.
