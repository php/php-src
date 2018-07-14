--TEST--
Test json_decode() result is iterable
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

function iter(iterable $items)
{
    foreach ($items as $key => $item) {
        echo "[{$key}] = '{$item}'" . PHP_EOL;
    }
}

$result = json_decode('{"one": "1", "two": "2"}', false, 512, JSON_AS_ARRAY_OBJECT);
iter($result);

?>
--EXPECT--
[one] = '1'
[two] = '2'