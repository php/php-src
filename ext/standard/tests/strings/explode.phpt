--TEST--
explode() function
--INI--
error_reporting=2047
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit only");
--FILE--
<?php
/* From http://bugs.php.net/19865 */
echo var_export(explode("\1", "a". chr(1). "b". chr(0). "d" . chr(1) . "f" . chr(1). "1" . chr(1) . "d"), TRUE);
echo "\n";
echo md5(var_export(explode("\1", "a". chr(1). "b". chr(0). "d" . chr(1) . "f" . chr(1). "1" . chr(1) . "d"), TRUE));
echo "\n";

try {
    var_dump(explode("", ""));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump(explode("", NULL));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump(explode(NULL, ""));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

var_dump(explode("a", ""));
var_dump(explode("a", "a"));
var_dump(explode("a", NULL));
try {
    var_dump(explode(NULL, "a"));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
var_dump(explode("abc", "acb"));
var_dump(explode("somestring", "otherstring"));
var_dump(explode("somestring", "otherstring", -1));
var_dump(explode("a", "aaaaaa"));
var_dump(explode("==", str_repeat("-=".ord(0)."=-", 10)));
var_dump(explode("=", str_repeat("-=".ord(0)."=-", 10)));
//////////////////////////////////////
var_dump(explode(":","a lazy dog:jumps:over:",-1));
var_dump(explode(":","a lazy dog:jumps:over", -1));
var_dump(explode(":","a lazy dog:jumps:over", -2));
var_dump(explode(":","a lazy dog:jumps:over:",-4));
var_dump(explode(":","a lazy dog:jumps:over:",-40000000000000));
var_dump(explode(":^:","a lazy dog:^:jumps::over:^:",-1));
var_dump(explode(":^:","a lazy dog:^:jumps::over:^:",-2));
?>
--EXPECT--
array (
  0 => 'a',
  1 => 'b' . "\0" . 'd',
  2 => 'f',
  3 => '1',
  4 => 'd',
)
d6bee42a771449205344c0938ad4f035
explode(): Argument #1 ($separator) cannot be empty
explode(): Argument #1 ($separator) cannot be empty
explode(): Argument #1 ($separator) cannot be empty
array(1) {
  [0]=>
  string(0) ""
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(0) ""
}
explode(): Argument #1 ($separator) cannot be empty
array(1) {
  [0]=>
  string(3) "acb"
}
array(1) {
  [0]=>
  string(11) "otherstring"
}
array(0) {
}
array(7) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
  [4]=>
  string(0) ""
  [5]=>
  string(0) ""
  [6]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(60) "-=48=--=48=--=48=--=48=--=48=--=48=--=48=--=48=--=48=--=48=-"
}
array(21) {
  [0]=>
  string(1) "-"
  [1]=>
  string(2) "48"
  [2]=>
  string(2) "--"
  [3]=>
  string(2) "48"
  [4]=>
  string(2) "--"
  [5]=>
  string(2) "48"
  [6]=>
  string(2) "--"
  [7]=>
  string(2) "48"
  [8]=>
  string(2) "--"
  [9]=>
  string(2) "48"
  [10]=>
  string(2) "--"
  [11]=>
  string(2) "48"
  [12]=>
  string(2) "--"
  [13]=>
  string(2) "48"
  [14]=>
  string(2) "--"
  [15]=>
  string(2) "48"
  [16]=>
  string(2) "--"
  [17]=>
  string(2) "48"
  [18]=>
  string(2) "--"
  [19]=>
  string(2) "48"
  [20]=>
  string(1) "-"
}
array(3) {
  [0]=>
  string(10) "a lazy dog"
  [1]=>
  string(5) "jumps"
  [2]=>
  string(4) "over"
}
array(2) {
  [0]=>
  string(10) "a lazy dog"
  [1]=>
  string(5) "jumps"
}
array(1) {
  [0]=>
  string(10) "a lazy dog"
}
array(0) {
}
array(0) {
}
array(2) {
  [0]=>
  string(10) "a lazy dog"
  [1]=>
  string(11) "jumps::over"
}
array(1) {
  [0]=>
  string(10) "a lazy dog"
}
