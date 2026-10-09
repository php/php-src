--TEST--
zlib.inflate filter removed by the error handler of its own notice
--EXTENSIONS--
zlib
--FILE--
<?php
$fp = fopen('php://memory', 'w+');
$filter = stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_WRITE);
set_error_handler(function (int $errno, string $errstr) use (&$filter) {
    echo $errstr, "\n";
    var_dump(stream_filter_remove($filter));
    return true;
});
var_dump(fwrite($fp, "\xff\xff\xff\xff"));

$fp = fopen('php://memory', 'w+');
fwrite($fp, "\xff\xff\xff\xff");
rewind($fp);
$filter = stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_READ);
var_dump(fread($fp, 10));
echo "Done\n";
?>
--EXPECT--
fwrite(): zlib: data error
bool(true)
bool(false)
fread(): zlib: data error
bool(true)
bool(false)
Done
