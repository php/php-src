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
Tick!
done
