--TEST--
OOM unserialize() array
--INI--
memory_limit=200M
--FILE--
<?php

$chunks = [];
$chunksize = 128;
for ($i = 0; $i < 2*1024*1024; $i++) {
    $chunkIdx = (int)($i / $chunksize);
    $chunks[$chunkIdx] = ($chunks[$chunkIdx] ?? '') . sprintf('i:%d;i:%d;', $i, $i);
}
$payload = 'a:'.$i.':{'.implode('', $chunks).'}';

ini_set('memory_limit', '100m');
var_dump(gettype(unserialize($payload)));

?>
--EXPECTF--
Fatal error: Allowed memory size of 104857600 bytes exhausted %S(tried to allocate %d bytes) in %s on line 12
