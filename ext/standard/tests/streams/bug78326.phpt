--TEST--
memory allocation on stream_get_contents()
--INI--
memory_limit=32M
--FILE--
<?php
$f = tmpfile();
fwrite($f, '.');

$chunks = array();
for ($i = 0; $i < 1000; ++$i) {
    rewind($f);
    $chunks[] = stream_get_contents($f, 1000000);
}
var_dump(count($chunks));
?>
--EXPECT--
int(1000)
