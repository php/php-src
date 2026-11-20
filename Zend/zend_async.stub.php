<?php

/** @generate-class-entries */

namespace Async {
    /**
     * @strict-properties
     * @not-serializable
     */
    final class Future
    {
        private function __construct() {}

        public function isDone(): bool {}
    }

    function async(callable $callback, mixed ...$args): Future {}

    function wait(Future $future): mixed {}

    function delay(float $seconds): Future {}

    function suspend(): void {}
}
