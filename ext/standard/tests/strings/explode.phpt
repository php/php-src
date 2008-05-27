--TEST--
explode() function
--INI--
error_reporting=2047
--FILE--
<?php
/* From http://bugs.php.net/19865 */
$s = (binary) var_export(explode(b"\1", (binary)"a". chr(1). "b". chr(0). "d" . chr(1) . "f" . chr(1). "1" . chr(1) . "d"), TRUE);
echo $s;
echo md5($s);
echo "\n";
var_dump(@explode("", ""));
var_dump(@explode("", NULL));
var_dump(@explode(NULL, ""));
var_dump(@explode("a", ""));
var_dump(@explode("a", "a"));
var_dump(@explode("a", NULL));
var_dump(@explode(NULL, a));
var_dump(@explode("abc", "acb"));
var_dump(@explode("somestring", "otherstring"));
var_dump(@explode("a", "aaaaaa"));
var_dump(@explode("==", str_repeat("-=".ord(0)."=-", 10)));
var_dump(@explode("=", str_repeat("-=".ord(0)."=-", 10)));
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
)d6bee42a771449205344c0938ad4f035
bool(false)
bool(false)
bool(false)
array(1) {
  [0]=>
  unicode(0) ""
}
array(2) {
  [0]=>
  unicode(0) ""
  [1]=>
  unicode(0) ""
}
array(1) {
  [0]=>
  unicode(0) ""
}
bool(false)
array(1) {
  [0]=>
  unicode(3) "acb"
}
array(1) {
  [0]=>
  unicode(11) "otherstring"
}
array(7) {
  [0]=>
  unicode(0) ""
  [1]=>
  unicode(0) ""
  [2]=>
  unicode(0) ""
  [3]=>
  unicode(0) ""
  [4]=>
  unicode(0) ""
  [5]=>
  unicode(0) ""
  [6]=>
  unicode(0) ""
}
array(1) {
  [0]=>
  unicode(60) "-=48=--=48=--=48=--=48=--=48=--=48=--=48=--=48=--=48=--=48=-"
}
array(21) {
  [0]=>
  unicode(1) "-"
  [1]=>
  unicode(2) "48"
  [2]=>
  unicode(2) "--"
  [3]=>
  unicode(2) "48"
  [4]=>
  unicode(2) "--"
  [5]=>
  unicode(2) "48"
  [6]=>
  unicode(2) "--"
  [7]=>
  unicode(2) "48"
  [8]=>
  unicode(2) "--"
  [9]=>
  unicode(2) "48"
  [10]=>
  unicode(2) "--"
  [11]=>
  unicode(2) "48"
  [12]=>
  unicode(2) "--"
  [13]=>
  unicode(2) "48"
  [14]=>
  unicode(2) "--"
  [15]=>
  unicode(2) "48"
  [16]=>
  unicode(2) "--"
  [17]=>
  unicode(2) "48"
  [18]=>
  unicode(2) "--"
  [19]=>
  unicode(2) "48"
  [20]=>
  unicode(1) "-"
}
array(3) {
  [0]=>
  unicode(10) "a lazy dog"
  [1]=>
  unicode(5) "jumps"
  [2]=>
  unicode(4) "over"
}
array(2) {
  [0]=>
  unicode(10) "a lazy dog"
  [1]=>
  unicode(5) "jumps"
}
array(1) {
  [0]=>
  unicode(10) "a lazy dog"
}
array(0) {
}
array(0) {
}
array(2) {
  [0]=>
  unicode(10) "a lazy dog"
  [1]=>
  unicode(11) "jumps::over"
}
array(1) {
  [0]=>
  unicode(10) "a lazy dog"
}
