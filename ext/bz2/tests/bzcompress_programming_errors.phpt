--TEST--
bzcompress(): providing invalid options
--EXTENSIONS--
bz2
--FILE--
<?php

$string = "Life it seems, will fade away
Drifting further everyday
Getting lost within myself
Nothing matters no one else";

try {
    var_dump(bzcompress($string, 0));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(bzcompress($string, 100));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(bzcompress($string, work_factor: -1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(bzcompress($string, work_factor: 255));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: bzcompress(): Argument #2 ($block_size) must be between 1 and 9
ValueError: bzcompress(): Argument #2 ($block_size) must be between 1 and 9
ValueError: bzcompress(): Argument #3 ($work_factor) must be between 0 and 250
ValueError: bzcompress(): Argument #3 ($work_factor) must be between 0 and 250
