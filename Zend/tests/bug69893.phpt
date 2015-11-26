--TEST--
Bug #69893: Strict comparison between integer and empty string keys crashes
--FILE--
<?php
var_dump([0 => 0] === ["" => 0]);
?>
--EXPECT--
bool(false)
