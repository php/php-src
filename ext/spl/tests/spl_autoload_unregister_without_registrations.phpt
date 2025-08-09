--TEST--
spl_autoload_unregister("spl_autoload_call") without registrations
--FILE--
<?php
var_dump(spl_autoload_unregister("spl_autoload_call"));
?>
Done
--EXPECTF--

Deprecated: spl_autoload_unregister(): Using spl_autoload_call() as a callback for spl_autoload_unregister() is deprecated, to remove all registered autoloaders, call spl_autoload_unregister() for all values returned from spl_autoload_functions() in %s on line %d
bool(true)
Done
