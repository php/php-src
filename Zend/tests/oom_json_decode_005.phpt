--TEST--
OOM json_decode() object as arrays
--INI--
memory_limit=10M
--FILE--
<?php

$chunks = [];
$chunksize = 64;
for ($i = 1; $i < 128*1024; $i++) {
    if (isset($chunks[$i % $chunksize])) {
        $chunks[$i % $chunksize] .= ',"k'.$i.'":0';
    } else {
        $chunks[$i % $chunksize] = '"k'.$i.'":0';
    }
}
$json = '{' . implode(',', $chunks) . '}';
var_dump(json_decode($json, associative: true));

?>
--EXPECTF--
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in %s on line 13
