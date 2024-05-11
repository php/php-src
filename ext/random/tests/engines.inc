<?php

namespace Random\Engine\Test;

use Random\Engine;

final class TestShaEngine implements Engine
{
    private string $state;

    public function __construct(?string $state = null)
    {
        if ($state !== null) {
            $this->state = $state;
        } else {
            $this->state = random_bytes(20);
        }
    }

    public function generate(): string
    {
        $this->state = sha1($this->state, true);

        return substr($this->state, 0, 8);
    }
}

final class TestWrapperEngine implements Engine
{
    private int $count = 0;

    public function __construct(private readonly Engine $engine)
    {
    }

    public function generate(): string
    {
        $this->count++;

        return $this->engine->generate();
    }

    public function getCount(): int
    {
        return $this->count;
    }
}

final class TestXoshiro128PlusPlusEngine implements Engine
{
    public function __construct(
        private int $s0,
        private int $s1,
        private int $s2,
        private int $s3
    ) {
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

        return pack('V', $result);
    }
}

final class TestCountingEngine32 implements Engine
{
    private int $count = 0;

    public function generate(): string
    {
        return pack('V', $this->count++);
    }
}

final class TestCountingEngine64 implements Engine
{
    private int $count = 0;

    public function generate(): string
    {
        if ($this->count > 2147483647 || $this->count < 0) {
            throw new \Exception('Overflow');
        }
        return pack('V', $this->count++) . "\x00\x00\x00\x00";
    }
}

?>
