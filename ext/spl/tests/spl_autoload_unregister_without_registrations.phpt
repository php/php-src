--TEST--
spl_autoload_unregister("spl_autoload_call") without registrations
--FILE--
<?php
var_dump(spl_autoload_unregister("spl_autoload_call"));
?>
Done
--EXPECT--
bool(true)
Done
