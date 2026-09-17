--TEST--
IntlCalendar get_debug_info() must not leak the time zone wrapper object
--EXTENSIONS--
intl
--FILE--
<?php
$cal = IntlCalendar::createInstance('UTC');
ob_start();
var_dump($cal);
ob_end_clean();

$o = new stdClass;
$before = spl_object_id($o);
unset($o);
for ($i = 0; $i < 10; $i++) {
    ob_start();
    var_dump($cal);
    ob_end_clean();
}
$o = new stdClass;
$after = spl_object_id($o);

var_dump($after - $before);
?>
--EXPECT--
int(0)
