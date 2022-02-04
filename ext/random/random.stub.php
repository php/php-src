<?php

/** @generate-class-entries */

namespace {
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
}

namespace Random\NumberGenerator
{
    class XorShift128Plus implements Random\NumberGenerator
    {
        public function __construct(int $seed) {}

        public function generate(): int {}

        public function __serialize(): array {}

        public function __unserialize(array $data): void {}
    }

    class MersenneTwister implements Random\NumberGenerator
    {
        public function __construct(int $seed, int $mode = MT_RAND_MT19937) {}

        /** @implementation-alias Random\NumberGenerator\XorShift128Plus::generate */
        public function generate(): int {}

        /** @implementation-alias Random\NumberGenerator\XorShift128Plus::__serialize */
        public function __serialize(): array {}
        
        /** @implementation-alias Random\NumberGenerator\XorShift128Plus::__unserialize */
        public function __unserialize(array $data): void {}
    }

    class CombinedLCG implements Random\NumberGenerator
    {
        /** @implementation-alias Random\NumberGenerator\XorShift128Plus::__construct */
        public function __construct(int $seed) {}

        /** @implementation-alias Random\NumberGenerator\XorShift128Plus::generate */
        public function generate(): int {}

        /** @implementation-alias Random\NumberGenerator\XorShift128Plus::__serialize */
        public function __serialize(): array {}
        
        /** @implementation-alias Random\NumberGenerator\XorShift128Plus::__unserialize */
        public function __unserialize(array $data): void {}
    }

    /** @not-serializable */
    class Secure implements Random\NumberGenerator
    {
        public function __construct() {}

        /** @implementation-alias Random\NumberGenerator\XorShift128Plus::generate */
        public function generate(): int {}
    }
}

namespace Random
{
    interface NumberGenerator
    {
        public function generate(): int;
    }

    final class Randomizer
    {
        private NumberGenerator $numberGenerator;

        public function __construct(private ?NumberGenerator $numberGenerator = null) {}

        public function getInt(int $min, int $max): int {}

        public function getBytes(int $legnth): string {}

        public function shuffleArray(array $array): array {}

        public function shuffleString(string $string): string {}

        public function __serialize(): array {}

        public function __unserialize(array $data): void {}
    }
}
