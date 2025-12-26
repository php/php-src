--TEST--
json_validate() throws with invalid flags
--EXTENSIONS--
json
--FILE--
<?php

try {
	json_validate("{}", 512, 9999999);
	echo "no exception\n";	
} catch (ValueError $e) {
	echo "caught\n";
}

?>
--EXPECT--
caught