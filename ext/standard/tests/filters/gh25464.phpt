--TEST--
GH-25464 (Segfault reading from php://memory after stream_filter_remove() on a read filter)
--EXTENSIONS--
zlib
--FILE--
<?php
mt_srand(1);
$data = '';
for ($i = 0; $i < 100000; $i++) {
    $data .= chr(mt_rand(0, 255));
}
$fp = fopen('php://memory', 'w+');
fwrite($fp, $data);
rewind($fp);
$filter = stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_READ);
fread($fp, 1);
stream_filter_remove($filter);
$rest = '';
while (($line = fgets($fp)) !== false) {
    $rest .= $line;
}
var_dump(str_ends_with($rest, substr($data, -8192)));
?>
--EXPECT--
bool(true)
