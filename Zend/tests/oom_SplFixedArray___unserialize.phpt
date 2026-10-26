--TEST--
OOM SplFixedArray::__unserialize()
--INI--
memory_limit=10M
--FILE--
<?php

$elems = [];
for ($i = 0; $i < 64*1024; $i++) {
    $elems[] = "i:$i;N;";
}
$payload = 'O:13:"SplFixedArray":' . count($elems) . ':{' . implode('', $elems) . '}';

$pad = str_repeat('a', max(10*1024*1024 - memory_get_usage(true) - 512*1024, 1));

unserialize($payload);

?>
--EXPECTF--
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in %s on line 11
