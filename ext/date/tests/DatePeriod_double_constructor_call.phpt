--TEST--
Double DatePeriod::__construct() call
--FILE--
<?php

$start = new \DateTime();
$interval = new \DateInterval('P1D');
$period = new \DatePeriod($start, $interval, 1);
$period->__construct($start, $interval, 1);

?>
===DONE===
--EXPECT--
===DONE===
