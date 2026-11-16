--TEST--
OOM unserialize() array of objects
--INI--
memory_limit=200M
--FILE--
<?php

$chunks = [];
$chunksize = 128;
for ($i = 0; $i < 512*1024; $i++) {
    $chunkIdx = (int)($i / $chunksize);
    $str = base_convert($i, 10, 36);
    $chunks[$chunkIdx] = ($chunks[$chunkIdx] ?? '') . sprintf('i:%d;O:8:"stdClass":0:{}', $i);
}
$payload = 'a:'.$i.':{'.implode('', $chunks).'}';

ini_set('memory_limit', '64m');
var_dump(gettype(unserialize($payload)));

?>
--EXPECTF--
Fatal error: Allowed memory size of 67108864 bytes exhausted %S(tried to allocate %d bytes) in %s on line 13
