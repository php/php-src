--TEST--
Bug #75031: Append mode in php://temp and php://memory
--FILE--
<?php

function test_75031($type, $mode) {
    $fp = fopen($type, $mode);
    fwrite($fp, "hello");
    fseek($fp, 0, SEEK_SET);
    fwrite($fp, "world");
    var_dump(stream_get_contents($fp, -1, 0));
    fclose($fp);
}

test_75031("php://temp", "w+");
test_75031("php://memory", "w+");
test_75031("php://temp", "a+");
test_75031("php://memory", "a+");

?>
--EXPECT--
string(5) "world"
string(5) "world"
string(10) "helloworld"
string(10) "helloworld"
