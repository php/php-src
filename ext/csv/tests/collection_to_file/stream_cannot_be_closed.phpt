--TEST--
Test Csv\collection_to_file(): userland cannot close the internal stream during iteration
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/collection_to_file_stream_cannot_be_closed.csv';

function rows(string $file): Generator {
    yield ['a', 'b'];
    /* A userland callback runs while the write stream is open: it can reach the stream
     * through the resource list, but must not be able to close it. */
    foreach (get_resources('stream') as $res) {
        if (stream_get_meta_data($res)['uri'] === $file) {
            var_dump(fclose($res));
            $GLOBALS['res'] = $res;
        }
    }
    yield ['c', 'd'];
}

Csv\collection_to_file($file, rows($file));
var_dump(gettype($GLOBALS['res']));
echo str_replace("\r\n", "<CRLF>\n", file_get_contents($file));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/collection_to_file_stream_cannot_be_closed.csv');
?>
--EXPECTF--
Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d
bool(false)
string(17) "resource (closed)"
a,b<CRLF>
c,d<CRLF>
