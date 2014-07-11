--TEST--
Wrong var_dump(DateInterval->days) value
--FILE--
<?php

$interval = new DateInterval('P2Y4DT6H8M');

var_dump($interval->days);
--EXPECT--
bool(false)
