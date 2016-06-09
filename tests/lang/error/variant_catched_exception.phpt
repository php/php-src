--TEST--
Catched exceptions should NOT be stored.
--FILE--
<?php

register_shutdown_function(function () {
	var_dump(error_get_last());
});

try {
	new DateTime('1/1/11111');
}
catch (Exception $e) {
	// Intentionally left blank.
}

?>
--EXPECTF--
NULL
