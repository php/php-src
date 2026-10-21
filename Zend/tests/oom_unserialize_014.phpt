--TEST--
OOM unserialize() object __unserialize (2)
--INI--
memory_limit=200M
--FILE--
<?php

class C {
    public function __unserialize($data) {
    }
}

$chunks = [];
$chunksize = 128;
for ($i = 0; $i < 1024*1024; $i++) {
    $chunkIdx = (int)($i / $chunksize);
    $prop = sprintf('p%d', $i);
    $chunks[$chunkIdx] = ($chunks[$chunkIdx] ?? '') . sprintf('s:%d:"%s";i:%d;', strlen($prop), $prop, $i);
}
$payload = 'O:1:"C":'.$i.':{'.implode('', $chunks).'}';
$chunks = null;

ini_set('memory_limit', '100m');
var_dump(gettype(unserialize($payload)));

?>
--EXPECTF--
Fatal error: Allowed memory size of 104857600 bytes exhausted %S(tried to allocate %d bytes) in %s on line 19
