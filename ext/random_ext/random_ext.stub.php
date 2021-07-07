<?php

/** @generate-class-entries */
/** @generate-function-entries */

namespace Random
{
    interface NumberGenerator
    {
        public function generate(): int;
    }
}

namespace Random\NumberGenerator
{
    class XorShift128Plus implements Random\NumberGenerator
    {
        public function __construct(?int $seed = null) {}

        public function generate(): int {}

        public function __serialize(): array {}

        public function __unserialize(array $data): void {}
    }

    class MT19937 implements Random\NumberGenerator
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

    class Secure implements Random\NumberGenerator
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
        // private Random\NumberGeneratorm $rng;
        private mixed $rng;

        public function __construct(?Random\NumberGenerator $rng = null) {}
        public function getNumberGenerator(): Random\NumberGenerator {}
        public function getInt(int $min, int $max): int {}
        public function getBytes(int $length): string {}
        public function shuffleArray(array $array): array {}
        public function shuffleString(string $string): string {}
        public function __serialize(): array {}
        public function __unserialize(array $data): void {}
    }
}
