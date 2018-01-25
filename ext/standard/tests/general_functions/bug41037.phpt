--TEST--
Bug #41037 (unregister_tick_function() inside the tick function crash PHP)
--FILE--
<?php

function a() {
		echo "hello";
			unregister_tick_function('a');
}

declare (ticks=1) {
    register_tick_function('a');

    echo "Done\n";
}
?>
--EXPECTF--
hello
Warning: unregister_tick_function(): Unable to delete tick function executed at the moment in %s on line %d
Done
hello
Warning: unregister_tick_function(): Unable to delete tick function executed at the moment in %s on line %d
