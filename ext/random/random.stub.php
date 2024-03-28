<?php

/** @generate-class-entries */

namespace {
    /**
     * @var int
     * @cvalue MT_RAND_MT19937
     */
    const MT_RAND_MT19937 = UNKNOWN;
    /**
     * @var int
     * @deprecated
     * @cvalue MT_RAND_PHP
     */
    const MT_RAND_PHP = UNKNOWN;

    function lcg_value(): float {}

    function mt_srand(?int $seed = null, int $mode = MT_RAND_MT19937): void {}

    /** @alias mt_srand */
    function srand(?int $seed = null, int $mode = MT_RAND_MT19937): void {}

    function rand(int $min = UNKNOWN, int $max = UNKNOWN): int {}

    function mt_rand(int $min = UNKNOWN, int $max = UNKNOWN): int {}

    function mt_getrandmax(): int {}

    /** @alias mt_getrandmax */
    function getrandmax(): int {}

    /** @refcount 1 */
    function random_bytes(int $length): string {}

    function random_int(int $min, int $max): int {}
}

namespace Random\Engine
{
    /**
     * @strict-properties
     */
    final class Mt19937 implements \Random\Engine
    {
        public function __construct(int|null $seed = null, int $mode = MT_RAND_MT19937) {}

        public function generate(): string {}

        public function __serialize(): array {}

        public function __unserialize(array $data): void {}

        public function __debugInfo(): array {}
    }

    /**
     * @strict-properties
     */
    final class PcgOneseq128XslRr64 implements \Random\Engine
    {
        public function __construct(string|int|null $seed = null) {}

        /** @implementation-alias Random\Engine\Mt19937::generate */
        public function generate(): string {}

        public function jump(int $advance): void {}

        /** @implementation-alias Random\Engine\Mt19937::__serialize */
        public function __serialize(): array {}

        /** @implementation-alias Random\Engine\Mt19937::__unserialize */
        public function __unserialize(array $data): void {}

        /** @implementation-alias Random\Engine\Mt19937::__debugInfo */
        public function __debugInfo(): array {}
    }

    /**
     * @strict-properties
     */
    final class Xoshiro256StarStar implements \Random\Engine
    {
        public function __construct(string|int|null $seed = null) {}

        /** @implementation-alias Random\Engine\Mt19937::generate */
        public function generate(): string {}

        public function jump(): void {}

        public function jumpLong(): void {}

        /** @implementation-alias Random\Engine\Mt19937::__serialize */
        public function __serialize(): array {}

        /** @implementation-alias Random\Engine\Mt19937::__unserialize */
        public function __unserialize(array $data): void {}

        /** @implementation-alias Random\Engine\Mt19937::__debugInfo */
        public function __debugInfo(): array {}
    }

    /**
     * @strict-properties
     * @not-serializable
     */
    final class Secure implements \Random\CryptoSafeEngine
    {
        /** @implementation-alias Random\Engine\Mt19937::generate */
        public function generate(): string {}
    }
}

namespace Random
{
    interface Engine
    {
        public function generate(): string;
    }

    interface CryptoSafeEngine extends Engine
    {
    }

    /**
     * @strict-properties
     */
    final class Randomizer
    {
        public readonly Engine $engine;

        public function __construct(?Engine $engine = null) {}

        public function nextInt(): int {}

        public function nextFloat(): float {}

        public function getFloat(float $min, float $max, IntervalBoundary $boundary = IntervalBoundary::ClosedOpen): float {}

        public function getInt(int $min, int $max): int {}

        public function getBytes(int $length): string {}

        public function getBytesFromString(string $string, int $length): string {}

        public function shuffleArray(array $array): array {}

        public function shuffleBytes(string $bytes): string {}

        /** @return array<int, string> */
        public function pickArrayKeys(array $array, int $num): array {}

        public function __serialize(): array {}

        public function __unserialize(array $data): void {}
    }

    enum IntervalBoundary {
        case ClosedOpen;
        case ClosedClosed;
        case OpenClosed;
        case OpenOpen;
    }

    /**
     * @strict-properties
     */
    class RandomError extends \Error
    {
    }

    /**
     * @strict-properties
     */
    class BrokenRandomEngineError extends RandomError
    {
    }

    /**
     * @strict-properties
     */
    class RandomException extends \Exception
    {
    }
}
