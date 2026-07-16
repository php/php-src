--TEST--
GH-22256 (Frameless calls evaluate arguments left-to-right)
--FILE--
<?php
$text = 'abc';
$offset = 0;
var_dump(substr($text, $offset, $offset = strpos($text, 'b', $offset)));

$offset = 0;
var_dump(substr($text, $offset, ++$offset));

function changeOffset(&$offset) {
    $offset = 1;
    return 1;
}

$offset = 0;
var_dump(substr($text, $offset, changeOffset($offset)));
?>
--EXPECT--
string(1) "a"
string(1) "a"
string(1) "a"
