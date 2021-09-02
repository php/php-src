<?php

/** @generate-class-entries */

namespace
{

    function lcg_value(): float {}

    function mt_srand(int $seed = 0, int $mode = MT_RAND_MT19937): void {}

    /** @alias mt_srand */
    function srand(int $seed = 0, int $mode = MT_RAND_MT19937): void {}

    function rand(int $min = UNKNOWN, int $max = UNKNOWN): int {}

    function mt_rand(int $min = UNKNOWN, int $max = UNKNOWN): int {}

    function mt_getrandmax(): int {}

    /** @alias mt_getrandmax */
    function getrandmax(): int {}

    /** @refcount 1 */
    function random_bytes(int $length): string {}

    function random_int(int $min, int $max): int {}

    final class Random
    {

        private Random\NumberGenerator $randomNumberGenerator;

        public function __construct(?Random\NumberGenerator $randomNumberGenerator = null) {}

        public function getNumberGenerator(): Random\NumberGenerator {}

        public function getInt(int $min, int $max): int {}

        public function getBytes(int $length): string {}

        public function shuffleArray(array $array): array {}

        public function shuffleString(string $string): string {}

        public function __serialize(): array {}

        public function __unserialize(array $data): void {}

    }

}

namespace Random
{

    abstract class NumberGenerator
    {

        abstract public function generate(): int {}

    }

}

namespace Random\NumberGenerator
{

    class XorShift128Plus extends Random\NumberGenerator
    {

        public function __construct(?int $seed = null) {}

        public function generate(): int {}

        public function __serialize(): array {}

        public function __unserialize(array $data): void {}

    }

    class MT19937 extends Random\NumberGenerator
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

    /**
     * @not-serializable
     */
    class Secure extends Random\NumberGenerator
    {

        /** @implementation-alias Random\NumberGenerator\XorShift128Plus::generate */
        public function generate(): int {}

    }

}
