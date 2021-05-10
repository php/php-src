--TEST--
Bug #66094 (unregister_tick_function tries to cast a Closure to a string)
--FILE--
<?php
declare(ticks=1);
register_tick_function($closure = function () { echo "Tick!\n"; });
unregister_tick_function($closure);
echo "done";
?>
--EXPECTF--
Deprecated: declare(ticks) is deprecated in %s on line %d

Deprecated: Function register_tick_function() is deprecated in %s on line %d
Tick!

Deprecated: Function unregister_tick_function() is deprecated in %s on line %d
done
