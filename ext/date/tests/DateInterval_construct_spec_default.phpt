--TEST--
DateInterval::__construct() default arg value (arg not supplied)
--FILE--
<?php

$interval = new DateInterval();

var_dump(
    $interval->y,
    $interval->m,
    $interval->d,
    $interval->h,
    $interval->i,
    $interval->s,
    $interval->f
);

?>
--EXPECT--
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
float(0)
