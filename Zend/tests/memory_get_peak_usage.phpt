--TEST--
int memory_get_peak_usage ([ bool $real_usage = false ] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--INI--
memory_limit=-1
--FILE--
<?php
var_dump($a = memory_get_peak_usage());
var_dump(memory_get_peak_usage(true));
var_dump(memory_get_peak_usage(false));
$array = range(1,1024*1024);
var_dump(memory_get_peak_usage() > $a);
?>
--EXPECTF--
int(%d)
int(%d)
int(%d)
bool(true)
