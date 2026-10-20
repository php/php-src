--TEST--
stream_set_timeout() rejects overflowing sums and preserves signed normalization
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8 && PHP_OS_FAMILY !== 'Windows') {
    die("skip timeval may be wider than PHP integers on 32-bit systems");
}
?>
--FILE--
<?php
class TimeoutStream
{
    public $context;
    public static array $timeout = [];

    public function stream_open($path, $mode, $options, &$openedPath): bool
    {
        return true;
    }

    public function stream_set_option($option, $seconds, $microseconds): bool
    {
        self::$timeout = [$seconds, $microseconds];
        return true;
    }
}

stream_wrapper_register('timeout', TimeoutStream::class);
$stream = fopen('timeout://test', 'r');
$maximum = PHP_OS_FAMILY === 'Windows' ? 2147483647 : PHP_INT_MAX;
$minimum = -$maximum - 1;

echo "overflow:\n";
foreach ([[$maximum, 1_000_000], [$minimum, -1_000_000]] as [$seconds, $microseconds]) {
    try {
        stream_set_timeout($stream, $seconds, $microseconds);
    } catch (ValueError $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump(TimeoutStream::$timeout === []);
}

echo "signed normalization and limits:\n";
foreach ([[-1, 0, -1, 0], [1, -2_000_000, -1, 0],
          [10, 2_345_678, 12, 345678], [10, -2_345_678, 8, -345678],
          [$maximum - 1, 1_000_000, $maximum, 0],
          [$minimum + 1, -1_000_000, $minimum, 0]] as [$seconds, $microseconds, $expectedSeconds, $expectedMicroseconds]) {
    var_dump(stream_set_timeout($stream, $seconds, $microseconds)
        && TimeoutStream::$timeout === [$expectedSeconds, $expectedMicroseconds]);
}
?>
--EXPECT--
overflow:
ValueError: stream_set_timeout(): Argument #2 ($seconds) is out of range when combined with argument #3 ($microseconds)
bool(true)
ValueError: stream_set_timeout(): Argument #2 ($seconds) is out of range when combined with argument #3 ($microseconds)
bool(true)
signed normalization and limits:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
