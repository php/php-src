--TEST--
proc_open() with no pipes
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) {
    // finished in 1.14s in a best-of-3 on an idle Intel Xeon X5670 on PHP 8.3.0-dev
    die("skip slow test");
}
?>
--FILE--
<?php

$spec = array();

$php = getenv("TEST_PHP_EXECUTABLE_ESCAPED");
$callee = escapeshellarg(__DIR__ . "/proc_open_pipes_sleep.inc");
proc_open("$php -n $callee", $spec, $pipes);

var_dump(count($spec));
var_dump($pipes);

?>
--EXPECT--
int(0)
array(0) {
}
