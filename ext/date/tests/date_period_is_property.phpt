--TEST--
Test isset on DatePeriod instantiated without its constructor
--FILE--
<?php

class MyDatePeriod extends DatePeriod {
    public int $my;
}

$rc = new ReflectionClass('MyDatePeriod');
$di = $rc->newInstanceWithoutConstructor();

var_dump(isset($di->start));
var_dump(empty($di->start));
var_dump(property_exists($di, "start"));

var_dump(isset($di->recurrences));
var_dump(empty($di->recurrences));
var_dump(property_exists($di, "recurrences"));

var_dump(isset($di->end));
var_dump(empty($di->end));
var_dump(property_exists($di, "end"));

var_dump(isset($di->my));
var_dump(empty($di->my));
var_dump(property_exists($di, "my"));

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
