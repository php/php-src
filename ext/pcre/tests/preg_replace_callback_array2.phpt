--TEST--
preg_replace_callback_array() errors
--FILE--
<?php

$a = array();
$b = "";

try {
    var_dump(preg_replace_callback_array(array("xx" => "s"), $a, -1, $b));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}


var_dump($b);
function f() {
    static $count = 1;
    throw new Exception($count);
}

var_dump(preg_replace_callback_array(array('/\w' => 'f'), 'z'));

try {
    var_dump(preg_replace_callback_array(array('/\w/' => 'f', '/.*/' => 'f'), 'z'));
} catch (Exception $e) {
    var_dump($e->getMessage());
}

echo "Done\n";
?>
--EXPECTF--
preg_replace_callback_array(): Argument #1 ($pattern) must contain only valid callbacks
string(0) ""

Warning: preg_replace_callback_array(): No ending delimiter '/' found in %spreg_replace_callback_array2.php on line %d
NULL
string(1) "1"
Done
