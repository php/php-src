--TEST--
GH-20043 (array_unique assertion failure with RC1 array causing an exception on sort)
--FILE--
<?php
try {
	array_unique([new stdClass, new stdClass], SORT_STRING | SORT_FLAG_CASE);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Object of class stdClass could not be converted to string
