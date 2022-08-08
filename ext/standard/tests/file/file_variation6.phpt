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
PHP Warning:  file(): '24' flag is not supported in %s on line %d
bool(false)
PHP Warning:  file(): '25' flag is not supported in %s on line %d
bool(false)
PHP Warning:  file(): '26' flag is not supported in %s on line %d
bool(false)
PHP Warning:  file(): '27' flag is not supported in %s on line %d
bool(false)
PHP Warning:  file(): '28' flag is not supported in %s on line %d
bool(false)
PHP Warning:  file(): '29' flag is not supported in %s on line %d
bool(false)
PHP Warning:  file(): '30' flag is not supported in %s on line %d
bool(false)
PHP Warning:  file(): '31' flag is not supported in %s on line %d
bool(false)
PHP Warning:  file(): '32' flag is not supported in %s on line %d
bool(false)
