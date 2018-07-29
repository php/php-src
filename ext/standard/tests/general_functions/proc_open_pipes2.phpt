--TEST--
proc_open() with no pipes
--SKIPIF--
<?php if (!function_exists('proc_open')) die ('skip proc_open function not available'); ?>
--FILE--
<?php

include dirname(__FILE__) . "/proc_open_pipes.inc";

$spec = array();

$php = getenv("TEST_PHP_EXECUTABLE");
$callee = create_sleep_script();
proc_open("$php -n $callee", $spec, $pipes);

var_dump(count($spec));
var_dump($pipes);

?>
--CLEAN--
<?php
include dirname(__FILE__) . "/proc_open_pipes.inc";

unlink_sleep_script();

?>
--EXPECT--
int(0)
array(0) {
}
