--TEST--
Throwing onCreate stops wildcard filter factory fallback
--EXTENSIONS--
iconv
--FILE--
<?php
class ThrowingFilter extends php_user_filter {
    public function onCreate(): bool {
        throw new RuntimeException('creation failed');
    }
}

class RefusedFilter extends php_user_filter {
    public function onCreate(): bool {
        return false;
    }
}

stream_filter_register('convert.iconv.UTF-8.*', ThrowingFilter::class);
stream_filter_register('convert.iconv.ISO-8859-1.*', RefusedFilter::class);
foreach (['UTF-8', 'ISO-8859-1'] as $encoding) {
    echo $encoding, "\n";
    $stream = fopen('php://memory', 'w+');
    try {
        stream_filter_append($stream, "convert.iconv.$encoding.UTF-16LE", STREAM_FILTER_WRITE);
    } catch (RuntimeException $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    fwrite($stream, 'A');
    rewind($stream);
    echo bin2hex(stream_get_contents($stream)), "\n";
    fclose($stream);
}
?>
--EXPECT--
UTF-8
RuntimeException: creation failed
41
ISO-8859-1
4100
