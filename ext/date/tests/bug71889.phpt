--TEST--
Bug #71889 (DateInterval::format segfault on '%' input)
--INI--
date.timezone=US/Eastern
--FILE--
<?php
$di = new DateInterval('P1D');
var_dump($di->format("%"));
?>
--EXPECT--
string(0) ""
