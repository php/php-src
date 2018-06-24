--TEST--
__compareTo: Objects implementing compare_objects handler are unaffected
--SKIPIF--
<?php
if (!class_exists('DateTime')) {
    echo 'skip DateTime not available';
}
?>
--FILE--
<?php

date_default_timezone_set('America/Vancouver');

$a = new DateTime('2015-01-01');
$b = new DateTime('2015-02-01');

var_dump($a <=> $a);
var_dump($a <=> $b);
var_dump($b <=> $a);
var_dump($b <=> $b);

?>
--EXPECTF--
int(0)
int(-1)
int(1)
int(0)
