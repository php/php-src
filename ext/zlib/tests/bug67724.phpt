--TEST--
Bug #67724 (chained zlib filters silently fail with large amounts of data)
--SKIPIF--
<?php
extension_loaded("zlib") or die("skip need ext/zlib");
?>
--FILE--
<?php
echo "Test\n";

$f = fopen(__DIR__."/bug67724.gz.gz", "rb")
    or die(current(error_get_last()));
stream_filter_append($f, "zlib.inflate", STREAM_FILTER_READ, ["window" => 30]);
stream_filter_append($f, "zlib.inflate", STREAM_FILTER_READ, ["window" => 30]);
for ($i = 0; !feof($f); $i += strlen(fread($f, 0x1000)))
    ;
fclose($f);

var_dump($i);

?>
DONE
--EXPECT--
Test
int(25600000)
DONE
