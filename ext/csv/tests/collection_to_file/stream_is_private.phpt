--TEST--
Test Csv\collection_to_file(): the internal stream is not exposed to userland during iteration
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/collection_to_file_private_stream.csv';
$before = count(get_resources('stream'));

function rows(int $before): Generator {
    yield ['a', 'b'];
    /* A userland callback runs while the write stream is open: it must not be able
     * to see (or close) that stream through the resource list. */
    var_dump(count(get_resources('stream')) === $before);
    yield ['c', 'd'];
}

Csv\collection_to_file($file, rows($before));
echo str_replace("\r\n", "<CRLF>\n", file_get_contents($file));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/collection_to_file_private_stream.csv');
?>
--EXPECT--
bool(true)
a,b<CRLF>
c,d<CRLF>
