--TEST--
Array variable shorthand preserves undefined variable and missing key warnings
--FILE--
<?php
set_error_handler(static function ($severity, $message) {
    echo $message, "\n";
    return true;
});

$array = [:$missing];
echo json_encode($array), "\n";
[:$name] = [];
var_dump($name);
list(:$other) = [];
var_dump($other);

$rows = [[]];
foreach ($rows as [:$item]) {
    var_dump($item);
}
?>
--EXPECT--
Undefined variable $missing
{"missing":null}
Undefined array key "name"
NULL
Undefined array key "other"
NULL
Undefined array key "item"
NULL
