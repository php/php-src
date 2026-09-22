--TEST--
zlib.deflate write filter: write_seek_mode parameter
--EXTENSIONS--
zlib
--FILE--
<?php
$file = __DIR__ . '/zlib_filter_write_seek_modes.zlib';

$text1 = 'First message that will be discarded.';
$text2 = 'Second message that replaces the first.';

/* "reset": ftruncate(0) + fseek(0) starts a fresh deflate stream */
$fp = fopen($file, 'w+');
stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE,
    ['write_seek_mode' => 'reset']);
fwrite($fp, $text1);
ftruncate($fp, 0);
var_dump(fseek($fp, 0, SEEK_SET) === 0);
fwrite($fp, $text2);
fclose($fp);

$fp = fopen($file, 'r');
stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_READ);
$decoded = stream_get_contents($fp);
fclose($fp);
var_dump($decoded === $text2);

/* "reset": only seekable to start */
$fp = fopen($file, 'w+');
stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE,
    ['write_seek_mode' => 'reset']);
fwrite($fp, $text1);
var_dump(fseek($fp, 5, SEEK_SET) === 0);
fclose($fp);

/* "preserve": same as default; seeks accepted, state preserved */
$fp = fopen($file, 'w+');
stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE,
    ['write_seek_mode' => 'preserve']);
fwrite($fp, $text1);
var_dump(fseek($fp, 0, SEEK_SET) === 0);
var_dump(fseek($fp, 50, SEEK_SET) === 0);
fclose($fp);

/* "strict": every write-chain seek fails */
$fp = fopen($file, 'w+');
stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE,
    ['write_seek_mode' => 'strict']);
fwrite($fp, $text1);
var_dump(@fseek($fp, 0, SEEK_SET) === -1);
var_dump(@fseek($fp, 50, SEEK_SET) === -1);
fclose($fp);

/* Invalid mode: ValueError */
$fp = fopen($file, 'w+');
stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE,
    ['write_seek_mode' => 'rewind']);
fclose($fp);

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/zlib_filter_write_seek_modes.zlib');
?>
--EXPECTF--
bool(true)
bool(true)

Warning: fseek(): Stream filter zlib.deflate is seekable only to start position in %s
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: stream_filter_append(): "write_seek_mode" filter parameter must be one of "preserve", "reset", or "strict" in %s

Warning: stream_filter_append(): Unable to create or locate filter "zlib.deflate" in %s
