--TEST--
Bug #46882 (Serialize / Unserialize misbehaviour under OS with different bit numbers)
--FILE--
<?php
var_dump(unserialize('i:5000000000;') == 5000000000);
?>
--EXPECT--
bool(true)
