--TEST--
bzip2 filter error on unknown filter name
--EXTENSIONS--
bz2
--FILE--
<?php
$fp = fopen('php://stdout', 'w');

var_dump(stream_filter_append($fp, 'bzip2.i_dont_exist', STREAM_FILTER_WRITE));

fclose($fp);

?>
--EXPECTF--
Warning: stream_filter_append(): Unable to create or locate filter "bzip2.i_dont_exist" in %s on line %d
bool(false)
