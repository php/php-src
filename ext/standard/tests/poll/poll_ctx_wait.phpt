--TEST--
Io\Poll\Context::wait(): Parameter validation
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

$poll_ctx = new Io\Poll\Context();

try {
    $poll_ctx->wait(timeout: Time\Duration::fromSeconds(1)->negate());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $poll_ctx->wait(maxEvents: -1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: Io\Poll\Context::wait(): Argument #1 ($timeout) must not be negative
ValueError: Io\Poll\Context::wait(): Argument #2 ($maxEvents) must be greater than 0
