--TEST--
GH-25464 (Segfault reading from php://memory after stream_filter_remove() on a read filter)
--EXTENSIONS--
zlib
--FILE--
<?php
$fp = fopen('php://memory', 'w+');
fwrite($fp, random_bytes(100000));
rewind($fp);
$filter = stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_READ);
fread($fp, 1);
stream_filter_remove($filter);
while (fgets($fp) !== false);
echo 'done', PHP_EOL;
?>
--EXPECT--
done
