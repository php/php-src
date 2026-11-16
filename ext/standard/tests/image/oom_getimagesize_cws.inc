<?php

function generate_file(string $path) {
    $file = fopen($path, 'wb');
    $uncompressedSize = 320 * 1024 * 1024;
    fwrite($file, "CWS\x06" . pack('V', $uncompressedSize + 8));
    $deflate = deflate_init(ZLIB_ENCODING_DEFLATE, ['level' => 9]);
    $chunk = str_repeat('A', 1024 * 1024);
    for ($i = 0; $i < 320; $i++) {
        fwrite($file, deflate_add(
            $deflate,
            $chunk,
            $i === 319 ? ZLIB_FINISH : ZLIB_NO_FLUSH,
        ));
    }
    fclose($file);
}
