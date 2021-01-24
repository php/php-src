--TEST--
Test [: &$value] alias for ['value' => &$value]
--FILE--
<?php

// In every array syntax where "'key' => " can be used, "key: " can be used instead.
$value = 'example';
$success = true;
// equals ['value' => &$value, 'success' => &$success]
$result = [: &$value, : &$success];
var_dump($result);

function destructure_result_by_reference(array $array) {
    // equals `['success' => $success, 'value' => $value] = $array;`
    [
        : &$success,
        : &$value,
    ] = $array;
    $value = 'modified';
    $success = null;

    printf("Modified array = %s\n", json_encode($array));
}
destructure_result_by_reference($result);
?>
--EXPECT--
array(2) {
  ["value"]=>
  &string(7) "example"
  ["success"]=>
  &bool(true)
}
Modified array = {"value":"modified","success":null}