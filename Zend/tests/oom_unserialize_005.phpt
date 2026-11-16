--TEST--
OOM unserialize() object
--INI--
memory_limit=200M
error_reporting=E_ALL&~E_DEPRECATED
--FILE--
<?php

$chunks = [];
$chunksize = 128;
for ($i = 0; $i < 1024*1024; $i++) {
    $chunkIdx = (int)($i / $chunksize);
    $prop = sprintf('p%d', $i);
    $chunks[$chunkIdx] = ($chunks[$chunkIdx] ?? '') . sprintf('s:%d:"%s";i:%d;', strlen($prop), $prop, $i);
}
$payload = 'O:8:"stdClass":'.$i.':{'.implode('', $chunks).'}';

ini_set('memory_limit', '100m');
var_dump(gettype(unserialize($payload)));

?>
--EXPECTF--
Fatal error: Allowed memory size of 104857600 bytes exhausted %S(tried to allocate %d bytes) in %s on line 13
