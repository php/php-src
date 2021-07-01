<?php

/** @generate-class-entries */
/** @generate-function-entries */

namespace Random\NumberGenerator
{
    interface RandomNumberGenerator
    {
        public function generate(): int;
    }

    class XorShift128Plus implements RandomNumberGenerator
    {
        public function __construct(?int $seed = null) {}

        public function generate(): int {}

        public function __serialize(): array {}

        public function __unserialize(array $data): void {}
    }

    class MT19937 implements RandomNumberGenerator
    {
        /** @implementation-alias Random\NumberGenerator\XorShift128Plus::__construct */
        public function __construct(?int $seed = null) {}

        /** @implementation-alias Random\NumberGenerator\XorShift128Plus::generate */
        public function generate(): int {}

        /** @implementation-alias Random\NumberGenerator\XorShift128Plus::__serialize */
        public function __serialize(): array {}

        /** @implementation-alias Random\NumberGenerator\XorShift128Plus::__unserialize */
        public function __unserialize(array $data): void {}
    }

    class Secure implements RandomNumberGenerator
    {
        public function __construct() {}

        /** @implementation-alias Random\NumberGenerator\XorShift128Plus::generate */
        public function generate(): int {}
    }
}

namespace
{
    final class Random
    {
        // FIXME: stub generator (gen_stub.php) does not supported.
        // private Random\NumberGenerator\RandomNumberGenerator $rng;
        private mixed $rng;

        public function __construct(?Random\NumberGenerator\RandomNumberGenerator $rng = null) {}
        public function getNumberGenerator(): Random\NumberGenerator\RandomNumberGenerator {}
        public function nextInt(): int {}
        public function getInt(int $min, int $max): int {}
        public function getBytes(int $length): string {}
        public function shuffleArray(array $array): array {}
        public function shuffleString(string $string): string {}
        public function __serialize(): array {}
        public function __unserialize(array $data): void {}
    }
}
