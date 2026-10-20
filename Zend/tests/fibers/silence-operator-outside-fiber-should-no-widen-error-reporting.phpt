--TEST--
Silence operator must not change the error_reporting value within the fiber
--FILE--
<?php

error_reporting(E_WARNING);

@(new Fiber(function () {
	var_dump(E_ALL);
	var_dump(E_WARNING);
    var_dump(error_reporting());
}))->start();

?>
--EXPECT--
int(30719)
int(2)
int(2)
