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
        return [];
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
printf(): Argument #1 ($format) must be of type string, stdClass given


Try 2:
Object of class stdClass could not be converted to string


Object with bad __toString():
Try 1:
badToString::__toString(): Return value must be of type string, array returned


Try 2:
badToString::__toString(): Return value must be of type string, array returned
