--TEST--
Pipe assign operator single-evaluation semantics
--FILE--
<?php

$counter = 0;
function track(): int {
    global $counter;
    $counter++;
    echo "track() called (call #", $counter, ")", PHP_EOL;
    return 0;
}

$arr = ["hello"];
$arr[track()] |>= strtoupper(...);

echo "track() was called ", $counter, " time(s)", PHP_EOL;
var_dump($arr);

?>
--EXPECT--
track() called (call #1)
track() was called 1 time(s)
array(1) {
  [0]=>
  string(5) "HELLO"
}
