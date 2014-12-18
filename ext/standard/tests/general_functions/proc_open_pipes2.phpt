--TEST--
proc_open() with no pipes 
--FILE--
<?php

$spec = array();
proc_open('sleep 1', $spec, $pipes);

var_dump(count($spec));
var_dump($pipes);

?>
--EXPECTF--
int(0)
array(0) {
}
