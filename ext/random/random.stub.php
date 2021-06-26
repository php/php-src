<?php

/** @generate-class-entries */
/** @generate-function-entries */

namespace Random\NumberGenerator
{
    interface RandomNumberGenerator
    {
        /** 
         * @tentative-return-type
         * @internal
         */
        public function generate(): int;
    }

    class XorShift128Plus implements RandomNumberGenerator
    {
        /** @tentative-return-type */
        public function __construct(?int $seed = null) {}

        /** @tentative-return-type */
        public function generate(): int {}

        /** @tentative-return-type */
        public function __serialize(): array {}

        /** @tentative-return-type */
        public function __unserialize(array $data): void {}
    }

    class MT19937 implements RandomNumberGenerator
    {
        /** 
         * @implementation-alias Random\NumberGenerator\XorShift128Plus::__construct 
         * @tentative-return-type
         */
        public function __construct(?int $seed = null) {}

        /** 
         * @implementation-alias Random\NumberGenerator\XorShift128Plus::generate 
         * @tentative-return-type
         */
        public function generate(): int {}

        /** 
         * @implementation-alias Random\NumberGenerator\XorShift128Plus::__serialize
         * @tentative-return-type
         */
        public function __serialize(): array {}

        /** 
         * @implementation-alias Random\NumberGenerator\XorShift128Plus::__unserialize
         * @tentative-return-type
         */
        public function __unserialize(array $data): void {}
    }

    class Secure implements RandomNumberGenerator
    {
        /** @tentative-return-type */
        public function __construct() {}

        /** 
         * @implementation-alias Random\NumberGenerator\XorShift128Plus::generate 
         * @tentative-return-type
         */
        public function generate(): int {}
    }
}

namespace
{
    interface RandomInterface
    {
        /** @tentative-return-type */
        public function nextInt(): int;

        /** @tentative-return-type */
        public function getInt(int $min, int $max): int;
        
        /** @tentative-return-type */
        public function getBytes(int $length): string;
        
        /** @tentative-return-type */
        public function shuffleArray(array $array): array;
        
        /** @tentative-return-type */
        public function shuffleString(string $string): string;
    }

    final class Random implements RandomInterface
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
