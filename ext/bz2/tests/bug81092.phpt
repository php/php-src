--TEST--
Bug #81092 (fflush before stream_filter_remove corrupts stream)
--EXTENSIONS--
bz2
--FILE--
<?php
$stream = fopen(__DIR__ . "/81092.bz2", 'wb+');
$filter = stream_filter_append($stream, 'bzip2.compress', STREAM_FILTER_WRITE, ['blocks' => 9, 'work' => 0]);
fwrite($stream, random_bytes(8192));
fflush($stream);
stream_filter_remove($filter);

var_dump(strlen(bzdecompress(file_get_contents(__DIR__ . "/81092.bz2"))));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/81092.bz2");
?>
--EXPECT--
int(8192)
