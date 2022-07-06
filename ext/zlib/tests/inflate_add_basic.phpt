--TEST--
Test incremental inflate_add() functionality
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
    print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
function inflateStream($mode, $flushSize) {
    $buffer = "";
    $inflated = null;
    $resource = inflate_init($mode);

    while (true) {
        $dataToInflate = yield $inflated;
        if (isset($dataToInflate)) {
            $buffer .= $dataToInflate;
            if (strlen($buffer) >= $flushSize) {
                $inflated = inflate_add($resource, $buffer);
                $buffer = "";
            } else {
                $inflated = null;
            }
        } else {
            $inflated = inflate_add($resource, $buffer, ZLIB_FINISH);
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
    'ZLIB_BLOCK' => ZLIB_BLOCK,
];

$uncompressed = "";
for ($i=0;$i<(32768*2);$i++) {
    $uncompressed .= chr(rand(48,125));
}

foreach ($modes as $modeKey => $mode) {
    $compressed = zlib_encode($uncompressed, $mode);
    $compressedLen = strlen($compressed);
    foreach ($flushSizes as $flushSize) {
        foreach ($flushTypes as $flushTypeKey => $flushType) {
            $inflated = "";
            $stream = inflateStream($mode, $flushSize, $flushType);
            for ($i=0;$i<$compressedLen;$i++) {
                $inflated .= $stream->send($compressed[$i]);
            }
            $inflated .= $stream->send(null);
            if ($inflated !== $uncompressed) {
                echo "Error: {$modeKey} | {$flushSize} | {$flushTypeKey}\n";
            }
        }

    }
}

?>
===DONE===
--EXPECT--
===DONE===
