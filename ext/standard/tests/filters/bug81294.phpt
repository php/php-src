--TEST--
Bug #81294 (Segfault when removing a filter)
--EXTENSIONS--
zlib
--FILE--
<?php
$f = fopen(__DIR__ . "/bug81294.txt", "wb+");
$flt1 = stream_filter_append($f, "zlib.deflate", STREAM_FILTER_WRITE);
$flt2 = stream_filter_append($f, "string.rot13", STREAM_FILTER_WRITE);
fwrite($f, "test");
stream_filter_remove($flt1);
fwrite($f, "test");
stream_filter_remove($flt2);
rewind($f);
var_dump(urlencode(fread($f, 1024)));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/bug81294.txt");
?>
--EXPECT--
string(16) "%2BV-.%01%00grfg"
