<?php

/** @generate-class-entries */

namespace Time {
    /** @strict-properties */
    class TimeException extends \Exception { }

    /**
     * @strict-properties
     */
    final readonly class Duration
    {
        public readonly int $seconds;

        public readonly int $nanoseconds;

        public readonly bool $negative;

        private function __construct()
        {
        }

        public static function fromSeconds(int $seconds, int $nanoseconds = 0): Duration
        {
        }

        public static function fromNanoseconds(int $nanoseconds): Duration
        {
        }

        public static function fromMicroseconds(int $microseconds): Duration
        {
        }

        public static function fromMilliseconds(int $milliseconds): Duration
        {
        }

        public static function fromMinutes(int $minutes): Duration
        {
        }

        public static function fromHours(int $hours): Duration
        {
        }

        public static function fromIso8601DurationString(string $specification): Duration
        {
        }

        public function negate(): Duration
        {
        }

        public function absolute(): Duration
        {
        }

        public function add(Duration $duration): Duration
        {
        }

        public function sub(Duration $duration): Duration
        {
        }

        public function multiplyBy(int $factor): Duration
        {
        }

        public function divideBy(int $divisor): Duration
        {
        }

        public static function compare(Duration $a, Duration $b): int
        {
        }
    }
}
