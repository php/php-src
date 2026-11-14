--TEST--
GH-25053: Phar size checks use filtered content rather than the underlying file size
--EXTENSIONS--
phar
zlib
--SKIPIF--
<?php
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
if (PHP_INT_SIZE != 8) die('skip this test is for 64bit platforms only');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
?>
--ENV--
TEST_TIMEOUT=600
--INI--
phar.readonly=0
--FILE--
<?php
$src = __DIR__ . '/gh25053_filtered_stream.bin';
$size = 2**32 + 1;
$fp = fopen($src, 'w+b');
ftruncate($fp, $size);
fclose($fp);

foreach (['buildFromIterator', 'addFile'] as $method) {
    echo $method, "\n";
    $path = __DIR__ . '/gh25053_filtered_stream-' . $method . '.tar';
    $phar = new PharData($path);
    try {
        if ($method === 'buildFromIterator') {
            $fp = fopen($src, 'rb');
            stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_READ);
            try {
                $phar->buildFromIterator(new ArrayIterator(['data' => $fp]));
            } finally {
                fclose($fp);
            }
        } else {
            $phar->addFile('php://filter/read=zlib.deflate/resource=' . $src, 'data');
        }

        // Decode in chunks so the round trip does not allocate a 4 GiB string.
        $fp = fopen('phar://' . $path . '/data', 'rb');
        stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_READ);
        $length = 0;
        $allZero = true;
        while (!feof($fp)) {
            $chunk = fread($fp, 1024 * 1024);
            $length += strlen($chunk);
            $allZero = $allZero && strspn($chunk, "\0") === strlen($chunk);
        }
        fclose($fp);
        var_dump($length === $size && $allZero);
    } catch (BadMethodCallException $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh25053_filtered_stream.bin');
@unlink(__DIR__ . '/gh25053_filtered_stream-buildFromIterator.tar');
@unlink(__DIR__ . '/gh25053_filtered_stream-addFile.tar');
?>
--EXPECT--
buildFromIterator
bool(true)
addFile
bool(true)
