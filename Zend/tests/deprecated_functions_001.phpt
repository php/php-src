--TEST--
Declare deprecated function
--FILE--
<?php

deprecated function a() {
	echo "Called\n";
}

a();

?>
--EXPECTF--
Deprecated: Function a() is deprecated in %sdeprecated_functions_001.php on line 7
Called