--TEST--
GH-13569: GC buffer grows up to GC_MAX_BUF_SIZE when scanning WeakMaps
--FILE--
<?php

$wm = new WeakMap();
$objs = [];
for ($i = 0; $i < 30_000; $i++) {
    $objs[] = $obj = new stdClass;
    $wm[$obj] = $obj;
}

gc_collect_cycles();

$tmp = $wm;
$tmp = null;

gc_collect_cycles();
?>
==DONE==
--EXPECT--
==DONE==
