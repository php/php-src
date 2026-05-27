--TEST--
bzip2.compress write filter: write_seek_mode parameter
--EXTENSIONS--
bz2
--FILE--
<?php
$file = __DIR__ . '/bz2_filter_write_seek_modes.bz2';

$text1 = 'First message that will be discarded.';
$text2 = 'Second message that replaces the first.';

/* "reset" */
$fp = fopen($file, 'w+');
stream_filter_append($fp, 'bzip2.compress', STREAM_FILTER_WRITE,
    ['write_seek_mode' => 'reset']);
fwrite($fp, $text1);
ftruncate($fp, 0);
var_dump(fseek($fp, 0, SEEK_SET) === 0);
fwrite($fp, $text2);
fclose($fp);

$fp = fopen($file, 'r');
stream_filter_append($fp, 'bzip2.decompress', STREAM_FILTER_READ);
$decoded = stream_get_contents($fp);
fclose($fp);
var_dump($decoded === $text2);

/* "strict" */
$fp = fopen($file, 'w+');
stream_filter_append($fp, 'bzip2.compress', STREAM_FILTER_WRITE,
    ['write_seek_mode' => 'strict']);
fwrite($fp, $text1);
var_dump(@fseek($fp, 0, SEEK_SET) === -1);
fclose($fp);

/* Invalid mode: ValueError */
$fp = fopen($file, 'w+');
stream_filter_append($fp, 'bzip2.compress', STREAM_FILTER_WRITE,
    ['write_seek_mode' => 'nope']);
fclose($fp);

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/bz2_filter_write_seek_modes.bz2');
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)

Warning: stream_filter_append(): "write_seek_mode" filter parameter must be one of "preserve", "reset", or "strict" in %s

Warning: stream_filter_append(): Unable to create or locate filter "bzip2.compress" in %s
