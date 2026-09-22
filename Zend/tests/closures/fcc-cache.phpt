--TEST--
FCCs are cached and shared
--FILE--
<?php
var_dump(strlen(...) === strlen(...));
?>
--EXPECT--
bool(true)
