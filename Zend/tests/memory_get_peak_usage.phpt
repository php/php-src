--TEST--
int memory_get_peak_usage ([ bool $real_usage = false ] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--FILE--
<?php
var_dump(memory_get_peak_usage());
var_dump(memory_get_peak_usage(true));
var_dump(memory_get_peak_usage(false));
?>
--EXPECTF--
int(%d)
int(%d)
int(%d)
