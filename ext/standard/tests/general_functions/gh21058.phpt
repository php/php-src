--TEST--
GH-21058 (error_log() crash with null destination and message type 3)
--FILE--
<?php

try {
	error_log("test", 3, null);
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: Path must not be empty
