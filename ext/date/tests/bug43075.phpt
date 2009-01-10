--TEST--
Bug #43075 (Support 24 as hour)
--FILE--
<?php
date_default_timezone_set('UTC');
$d = date_create("2007-11-01T24:34:00+00:00");
echo $d->format("c");
?>
--EXPECT--
2007-11-02T00:34:00+00:00
