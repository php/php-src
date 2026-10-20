--TEST--
bzip2.decompress filter param errors
--EXTENSIONS--
bz2
--FILE--
<?php
$fp = fopen('php://stdout', 'w');

$param = 'not an array or bool';
var_dump(stream_filter_append($fp, 'bzip2.decompress', STREAM_FILTER_WRITE, $param));

fclose($fp);

?>
--EXPECTF--
Warning: stream_filter_append(): Filter parameters for bzip2.decompress filter must be of type array|object|bool, string given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "bzip2.decompress" in %s on line %d
bool(false)
