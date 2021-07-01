--TEST--
Test incremental deflate_add() functionality
--EXTENSIONS--
zlib
--FILE--
<?php

function deflateStream($mode, $flushSize, $flushType) {
    $buffer = "";
    $deflated = null;
    $resource = deflate_init($mode);

    while (true) {
        $dataToCompress = yield $deflated;
        if (isset($dataToCompress)) {
            $buffer .= $dataToCompress;
            if (strlen($buffer) >= $flushSize) {
                $deflated = deflate_add($resource, $buffer, $flushType);
                $buffer = "";
            } else {
                $deflated = null;
            }
        } else {
            $deflated = deflate_add($resource, $buffer, ZLIB_FINISH);
        }
    }
}

$modes = [
    'ZLIB_ENCODING_RAW'     => ZLIB_ENCODING_RAW,
    'ZLIB_ENCODING_GZIP'    => ZLIB_ENCODING_GZIP,
    'ZLIB_ENCODING_DEFLATE' => ZLIB_ENCODING_DEFLATE,
];
$flushSizes = [1, 4, 32768];
$flushTypes = [
    'ZLIB_SYNC_FLUSH' => ZLIB_SYNC_FLUSH,
    'ZLIB_PARTIAL_FLUSH' => ZLIB_PARTIAL_FLUSH,
    'ZLIB_FULL_FLUSH' => ZLIB_FULL_FLUSH,
    'ZLIB_NO_FLUSH' => ZLIB_NO_FLUSH,
];

/* Z_BLOCK is only available for deflate when built against zlib >= 1.2.4 */
if (ZLIB_VERNUM >= 0x1240) {
    $flushTypes['ZLIB_BLOCK'] = ZLIB_BLOCK;
}

foreach ($modes as $modeKey => $mode) {
    foreach ($flushSizes as $flushSize) {
        foreach ($flushTypes as $flushTypeKey => $flushType) {
            $uncompressed = $compressed = "";
            $stream = deflateStream($mode, $flushSize, $flushType);
            foreach (range("a", "z") as $c) {
                $uncompressed .= $c;
                $compressed .= $stream->send($c);
            }
            $compressed .= $stream->send(null);
            if ($uncompressed !== zlib_decode($compressed)) {
                echo "Error: {$modeKey} | {$flushSize} | {$flushTypeKey}\n";
            }
        }
    }
}
?>
===DONE===
--EXPECT--
===DONE===
