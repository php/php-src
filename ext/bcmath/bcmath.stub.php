<?php

/** @generate-class-entries */

namespace
{
    /** @refcount 1 */
    function bcadd(string $num1, string $num2, ?int $scale = null): string {}

    /** @refcount 1 */
    function bcsub(string $num1, string $num2, ?int $scale = null): string {}

    /** @refcount 1 */
    function bcmul(string $num1, string $num2, ?int $scale = null): string {}

    /** @refcount 1 */
    function bcdiv(string $num1, string $num2, ?int $scale = null): string {}

    /** @refcount 1 */
    function bcmod(string $num1, string $num2, ?int $scale = null): string {}

    /**
     * @return string[]
     * @refcount 1
     */
    function bcdivmod(string $num1, string $num2, ?int $scale = null): array {}

    /** @refcount 1 */
    function bcpowmod(string $num, string $exponent, string $modulus, ?int $scale = null): string {}

    /** @refcount 1 */
    function bcpow(string $num, string $exponent, ?int $scale = null): string {}

    /** @refcount 1 */
    function bcsqrt(string $num, ?int $scale = null): string {}

    function bccomp(string $num1, string $num2, ?int $scale = null): int {}

    function bcscale(?int $scale = null): int {}

    /** @refcount 1 */
    function bcfloor(string $num): string {}

    /** @refcount 1 */
    function bcceil(string $num): string {}

    /** @refcount 1 */
    function bcround(string $num, int $precision = 0, RoundingMode $mode = RoundingMode::HalfAwayFromZero): string {}
}

namespace BcMath
{
    /** @strict-properties */
    final readonly class Number implements \Stringable
    {
        /** @virtual */
        public string $value;
        /** @virtual */
        public int $scale;

        public function __construct(string|int $num) {}

        public function add(Number|string|int $num, ?int $scale = null): Number {}

        public function sub(Number|string|int $num, ?int $scale = null): Number {}

        public function mul(Number|string|int $num, ?int $scale = null): Number {}

        public function div(Number|string|int $num, ?int $scale = null): Number {}

        public function mod(Number|string|int $num, ?int $scale = null): Number {}

        /** @return Number[] */
        public function divmod(Number|string|int $num, ?int $scale = null): array {}

        public function powmod(Number|string|int $exponent, Number|string|int $modulus, ?int $scale = null): Number {}

        public function pow(Number|string|int $exponent, ?int $scale = null): Number {}

        public function sqrt(?int $scale = null): Number {}

        public function floor(): Number {}

        public function ceil(): Number {}

        public function round(int $precision = 0, \RoundingMode $mode = \RoundingMode::HalfAwayFromZero): Number {}

        public function compare(Number|string|int $num, ?int $scale = null): int {}

        public function __toString(): string {}

        public function __serialize(): array {}

        public function __unserialize(array $data): void {}
    }
}
