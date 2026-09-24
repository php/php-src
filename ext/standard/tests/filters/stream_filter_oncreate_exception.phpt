--TEST--
Throwing onCreate does not attach a stream filter
--FILE--
<?php
class ThrowingFilter extends php_user_filter {
    public function onCreate(): bool {
        throw new RuntimeException('creation failed');
    }

    public function filter($in, $out, &$consumed, bool $closing): int {
        echo "filter called\n";
        return PSFS_ERR_FATAL;
    }

    public function onClose(): void {
        echo "onClose called\n";
    }

    public function __destruct() {
        echo "destroyed\n";
    }
}

stream_filter_register('throwing', ThrowingFilter::class);
foreach (['stream_filter_append', 'stream_filter_prepend'] as $attach) {
    echo $attach, "\n";
    $stream = fopen('php://memory', 'w+');
    try {
        $attach($stream, 'throwing', STREAM_FILTER_WRITE);
    } catch (RuntimeException $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump(fwrite($stream, 'Hello'));
    rewind($stream);
    var_dump(stream_get_contents($stream));
    fclose($stream);
}
?>
--EXPECT--
stream_filter_append
destroyed
RuntimeException: creation failed
int(5)
string(5) "Hello"
stream_filter_prepend
destroyed
RuntimeException: creation failed
int(5)
string(5) "Hello"
