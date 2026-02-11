--TEST--
GH-21058 (error_log() crash with null destination and message type 3)
--FILE--
<?php

try {
	error_log("test", 3, null);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Path must not be empty
