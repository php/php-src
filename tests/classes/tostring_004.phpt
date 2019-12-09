--TEST--
Object to string conversion: error cases and behaviour variations.
--FILE--
<?php
function test_error_handler($err_no, $err_msg, $filename, $linenum) {
        echo "Error: $err_no - $err_msg\n";
}
set_error_handler('test_error_handler');
error_reporting(8191);


echo "Object with no __toString():\n";
$obj = new stdClass;
echo "Try 1:\n";
try {
    printf($obj);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
printf("\n");

echo "\nTry 2:\n";
try {
    printf($obj . "\n");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "\n\nObject with bad __toString():\n";
class badToString {
	function __toString() {
		return 0;
	}
}

$obj = new badToString;
echo "Try 1:\n";
try {
    printf($obj);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
printf("\n");

echo "\nTry 2:\n";
try {
    printf($obj . "\n");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Object with no __toString():
Try 1:
printf() expects parameter 1 to be string, object given


Try 2:
Object of class stdClass could not be converted to string


Object with bad __toString():
Try 1:
Method badToString::__toString() must return a string value


Try 2:
Method badToString::__toString() must return a string value
