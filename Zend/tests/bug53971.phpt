--TEST--
Bug #53971 (isset() and empty() produce apparently spurious runtime error)
--FILE--
<?php
$s = "";
var_dump(isset($s[0][0]));
?>
--EXPECT--
bool(false)


