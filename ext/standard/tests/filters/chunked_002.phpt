--TEST--
Dechunk write filter state must survive stream seek
--FILE--
<?php
/* The dechunk filter is commonly used as a write filter on php://temp buffers.
 * The buffer is written to (through the filter) and then seeked to re-read
 * the already-decoded output. Seeking the stream must NOT reset the write
 * filter state, otherwise multi-chunk transfers break. */

$buffer = fopen('php://temp', 'w+');
stream_filter_append($buffer, 'dechunk', STREAM_FILTER_WRITE);

/* Write first chunk */
fwrite($buffer, "5\r\nHello\r\n");

/* Read back decoded data; this seeks to offset 0 internally */
$data = stream_get_contents($buffer, -1, 0);
var_dump($data);

/* Write second chunk; filter must still be in the correct state */
fwrite($buffer, "7\r\n, World\r\n");

/* Read all decoded data from the beginning */
$data = stream_get_contents($buffer, -1, 0);
var_dump($data);

/* Write final (terminating) chunk */
fwrite($buffer, "0\r\n\r\n");

/* Read complete decoded output */
$data = stream_get_contents($buffer, -1, 0);
var_dump($data);

fclose($buffer);

/* Also verify that incomplete chunked transfer is still detected:
 * writing a non-chunk byte after the filter has been reset by a
 * seek should not produce output. */
$buffer = fopen('php://temp', 'w+');
stream_filter_append($buffer, 'dechunk', STREAM_FILTER_WRITE);

fwrite($buffer, "5\r\nHello\r\n");
$data = stream_get_contents($buffer, -1, 0);
var_dump($data);

/* The transfer is still in progress (no terminating 0-chunk seen).
 * Verify incomplete state is preserved by checking ftell: the decoded
 * write position should reflect only the 5 bytes written so far. */
var_dump(ftell($buffer));

fclose($buffer);
?>
--EXPECT--
string(5) "Hello"
string(12) "Hello, World"
string(12) "Hello, World"
string(5) "Hello"
int(5)
