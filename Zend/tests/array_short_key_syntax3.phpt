--TEST--
Test [: $value] alias for ['value' => $value]
--FILE--
<?php

// In every array syntax where "'key' => " can be used, "key: " can be used instead.
$value = 'example';
$success = true;
$x = 1;
// equals ['value' => $value, 'success' => $success]
$result = [: $value, : $success, : $x];
echo json_encode($result), "\n";

function destructure_result(array $array) {
    // equals `['success' => $success, 'value' => $value] = $array;`
    [
        : $success,
        : $value,
    ] = $array;
    printf("Success=%s, value=%s\n", $success, json_encode($value));
}
destructure_result($result);
?>
--EXPECT--
{"value":"example","success":true,"x":1}
Success=1, value="example"