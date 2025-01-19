--TEST--
simplexml_load_stream() - errors
--EXTENSIONS--
simplexml
--FILE--
<?php

$tmp = fopen("php://memory", "w+");
try {
    simplexml_load_stream($tmp, "doesnotexist");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
fclose($tmp);

?>
--EXPECT--
simplexml_load_stream(): Argument #2 ($encoding) must be a valid character encoding
