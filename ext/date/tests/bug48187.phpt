--TEST--
Bug #48187 (DateTime::diff() corrupting microtime() result)
--FILE--
<?php
date_default_timezone_set('UTC');

// two arbitrary dates
$date1 = new DateTime('2005-07-23');
$date2 = new DateTime('2006-02-14');

$begin_u = microtime(true);
$begin_t = time();

if (microtime(true) - $begin_u < 1) {
    var_dump('microtime() difference less 1 second');
} else {
    var_dump('microtime() difference greater or equal 1 second');
}

if (time() - $begin_t < 1) {
    var_dump('time() difference less 1 second');
} else {
    var_dump('time() difference greater or equal 1 second');
}
?>
--EXPECT--
string(36) "microtime() difference less 1 second"
string(31) "time() difference less 1 second"
