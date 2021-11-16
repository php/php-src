--TEST--
Bug #74240 (deflate_add can allocate too much memory)
--EXTENSIONS--
zlib
--FILE--
<?php

ini_set('memory_limit', '64M');

$deflator = deflate_init(ZLIB_ENCODING_RAW);

$bytes = str_repeat("*", 65536);

// this crashes after about 500 iterations if PHP is
// configured for 64M
for ($i = 0; $i < 1000; $i++) {
    $output = deflate_add(
        $deflator,
        $bytes,
        ZLIB_SYNC_FLUSH
    );
}
echo "Completed\n";
?>
--EXPECT--
Completed
