--TEST--
hash(), hash_file(): invalid xxh3/xxh128 options throw for a large input instead of hanging
--FILE--
<?php
$file = __DIR__ . '/xxh3_invalid_options_large_input.bin';
file_put_contents($file, str_repeat('z', 5000));

foreach (['xxh3', 'xxh128'] as $algo) {
    foreach ([256, 300, 5000] as $n) {
        try {
            hash($algo, str_repeat('z', $n), false, ['secret' => 'short']);
        } catch (Throwable $e) {
            echo $algo, ' ', $n, ': ', $e::class, ': ', $e->getMessage(), "\n";
        }
    }
    try {
        hash($algo, str_repeat('z', 5000), false, ['seed' => 1, 'secret' => str_repeat('k', 136)]);
    } catch (Throwable $e) {
        echo $algo, ' seed and secret: ', $e::class, ': ', $e->getMessage(), "\n";
    }
    try {
        hash_file($algo, $file, false, ['secret' => 'short']);
    } catch (Throwable $e) {
        echo $algo, ' file: ', $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/xxh3_invalid_options_large_input.bin');
?>
--EXPECT--
xxh3 256: Error: xxh3: Secret length must be >= 136 bytes, 5 bytes passed
xxh3 300: Error: xxh3: Secret length must be >= 136 bytes, 5 bytes passed
xxh3 5000: Error: xxh3: Secret length must be >= 136 bytes, 5 bytes passed
xxh3 seed and secret: Error: xxh3: Only one of seed or secret is to be passed for initialization
xxh3 file: Error: xxh3: Secret length must be >= 136 bytes, 5 bytes passed
xxh128 256: Error: xxh128: Secret length must be >= 136 bytes, 5 bytes passed
xxh128 300: Error: xxh128: Secret length must be >= 136 bytes, 5 bytes passed
xxh128 5000: Error: xxh128: Secret length must be >= 136 bytes, 5 bytes passed
xxh128 seed and secret: Error: xxh128: Only one of seed or secret is to be passed for initialization
xxh128 file: Error: xxh128: Secret length must be >= 136 bytes, 5 bytes passed
