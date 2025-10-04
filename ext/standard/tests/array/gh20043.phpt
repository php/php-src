--TEST--
GH-20043 (array_unique assertion failure with RC1 array causing an exception on sort)
--FILE--
<?php
try {
	array_unique([new stdClass, new stdClass], SORT_STRING | SORT_FLAG_CASE);
} catch (Error $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
Object of class stdClass could not be converted to string
