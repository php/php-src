--TEST--

--FILE--
<?php
$test_string = str_repeat('Eins zwei drei', 2000);
$replaced = preg_replace('/\s/', '-', $test_string);
$mem0 = memory_get_usage();
$replaced = str_repeat($replaced, 1);
$mem1 = memory_get_usage();
var_dump($mem0 == $mem1);
?>
--EXPECT--
bool(true)
