--TEST--
Unexposed/leaked stream encloses another stream
--SKIPIF--
<?php
if (!function_exists('leak_variable')) die("skip only debug builds");
--FILE--
<?php
$s = fopen('php://temp/maxmemory=1024','wb+');

$t = fopen('php://temp/maxmemory=1024','wb+');

/* force conversion of inner stream to STDIO. */
$i = 0;
while ($i++ < 5000) {
    fwrite($t, str_repeat('a',1024));
}

leak_variable($s, true);
leak_variable($t, true);
--EXPECT--
