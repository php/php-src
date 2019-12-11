--TEST--
file() with a range of integer flag values
--FILE--
<?php

$filepath = __FILE__ . ".tmp";
$fd = fopen($filepath, "w+");
fwrite($fd, "Line 1\nLine 2\nLine 3");
fclose($fd);

for ($flags = 0; $flags <= 32; $flags++) {
    try {
	    var_dump(file($filepath, $flags));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}

unlink($filepath);

?>
--EXPECT--
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(6) "Line 1"
  [1]=>
  string(6) "Line 2"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(6) "Line 1"
  [1]=>
  string(6) "Line 2"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(6) "Line 1"
  [1]=>
  string(6) "Line 2"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(6) "Line 1"
  [1]=>
  string(6) "Line 2"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(6) "Line 1"
  [1]=>
  string(6) "Line 2"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(6) "Line 1"
  [1]=>
  string(6) "Line 2"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(6) "Line 1"
  [1]=>
  string(6) "Line 2"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(6) "Line 1"
  [1]=>
  string(6) "Line 2"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(6) "Line 1"
  [1]=>
  string(6) "Line 2"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(6) "Line 1"
  [1]=>
  string(6) "Line 2"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(6) "Line 1"
  [1]=>
  string(6) "Line 2"
  [2]=>
  string(6) "Line 3"
}
array(3) {
  [0]=>
  string(6) "Line 1"
  [1]=>
  string(6) "Line 2"
  [2]=>
  string(6) "Line 3"
}
'24' flag is not supported
'25' flag is not supported
'26' flag is not supported
'27' flag is not supported
'28' flag is not supported
'29' flag is not supported
'30' flag is not supported
'31' flag is not supported
'32' flag is not supported