--TEST--
GH-25053: Phar rejects compressed entries whose size exceeds the 32-bit size field
--EXTENSIONS--
phar
zlib
hash
--SKIPIF--
<?php
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
if (PHP_INT_SIZE != 8) die('skip this test is for 64bit platforms only');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
if (PHP_OS === 'FreeBSD') die('skip exceeds FreeBSD CI hard memory limit');
?>
--ENV--
TEST_TIMEOUT=600
--INI--
phar.readonly=0
--FILE--
<?php
// Deterministic incompressible data. The repeated block is larger than the
// DEFLATE history window, so repeating it does not make it compressible.
$block = '';
for ($i = 0; $i < 32768; $i++) {
    $block .= hash('sha256', pack('V', $i), true);
}

$src = __DIR__ . '/gh25053_compressed_size.bin';
$fp = fopen($src, 'wb');
// Leave room for ZIP headers so adding the uncompressed entry is valid.
// DEFLATE overhead then pushes the compressed size above UINT32_MAX.
$remaining = 2**32 - 65536;
while ($remaining > 0) {
    $length = min($remaining, strlen($block));
    if (fwrite($fp, substr($block, 0, $length)) !== $length) {
        throw new RuntimeException('Could not create the input file');
    }
    $remaining -= $length;
}
fclose($fp);
unset($block);

foreach (['phar', 'zip'] as $format) {
    echo $format, "\n";
    $path = __DIR__ . '/gh25053_compressed_size.' . $format;
    $phar = $format === 'phar' ? new Phar($path) : new PharData($path);
    $phar->startBuffering();
    $phar->addFile($src, 'data');

    try {
        $phar['data']->compress(Phar::GZ);
        $phar->stopBuffering();
        echo "Compression unexpectedly succeeded\n";
    } catch (PharException $e) {
        // The format limit must be reported, rather than silently narrowing
        // the compressed size and writing only that many bytes.
        echo $e::class, "\n";
    }
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh25053_compressed_size.bin');
@unlink(__DIR__ . '/gh25053_compressed_size.phar');
@unlink(__DIR__ . '/gh25053_compressed_size.zip');
?>
--EXPECT--
phar
PharException
zip
PharException
