--TEST--
Random: Engine: User: Xoshiro128++
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php

/*
 * Original code by Tim Düsterhus <tim@bastelstu.be>
 */

final class Xoshiro128PP implements \Random\Engine
{
    private function __construct(
        private int $s0,
        private int $s1,
        private int $s2,
        private int $s3
    ) {
    }

    public static function fromNumbers($s0, $s1, $s2, $s3)
    {
        return new self($s0, $s1, $s2, $s3);
    }

    private static function rotl($x, $k)
    {
        return (($x << $k) | ($x >> (32 - $k))) & 0xFFFFFFFF;
    }

    public function generate(): string
    {
        $result = (self::rotl(($this->s0 + $this->s3) & 0xFFFFFFFF, 7) + $this->s0) & 0xFFFFFFFF;

        $t = ($this->s1 << 9)  & 0xFFFFFFFF;

        $this->s2 ^= $this->s0;
        $this->s3 ^= $this->s1;
        $this->s1 ^= $this->s2;
        $this->s0 ^= $this->s3;

        $this->s2 ^= $t;

        $this->s3 = self::rotl($this->s3, 11);

        return \pack('V', $result);
    }
}

$g = Xoshiro128PP::fromNumbers(1, 2, 3, 4);

for ($i = 0; $i < 102400; $i++) {
    $g->generate();
}

die(\bin2hex($g->generate()));

?>
--EXPECT--
fa3c872c
