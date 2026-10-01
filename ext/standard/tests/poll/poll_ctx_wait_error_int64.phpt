--TEST--
Io\Poll\Context::wait(): Parameter validation upper limit
--SKIPIF--
<?php
if (PHP_INT_SIZE <= 4) {
    die("skip this test is for > 32bit platforms only");
}
?>
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

$poll_ctx = new Io\Poll\Context();

try {
    $poll_ctx->wait(maxEvents: PHP_INT_MAX);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: Io\Poll\Context::wait(): Argument #2 ($maxEvents) must be less than or equal to 2147483647
