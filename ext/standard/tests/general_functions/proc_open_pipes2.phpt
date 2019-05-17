--TEST--
proc_open() with no pipes
--FILE--
<?php

$spec = array();

$php = getenv("TEST_PHP_EXECUTABLE");
$callee = __DIR__ . "/proc_open_pipes_sleep.inc";
proc_open("$php -n $callee", $spec, $pipes);

var_dump(count($spec));
var_dump($pipes);

?>
--EXPECT--
int(0)
array(0) {
}
