--TEST--
Test Csv\row_to_array(): data after the end of the row is an error
--EXTENSIONS--
csv
--FILE--
<?php
var_dump(Csv\row_to_array("a,b"));
var_dump(Csv\row_to_array("a,b\r\n"));
foreach (["a,b\r\nc,d", "a,b\r\n\r\n", "\"a\r\nb\",c\r\nd,e\r\n"] as $row) {
    try {
        var_dump(Csv\row_to_array($row));
    } catch (\ValueError $e) {
        echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
    }
}
?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
ValueError: Csv\row_to_array(): Argument #1 ($row) must contain a single row
ValueError: Csv\row_to_array(): Argument #1 ($row) must contain a single row
ValueError: Csv\row_to_array(): Argument #1 ($row) must contain a single row
