--TEST--
explode() function
--POST--
--GET--
--INI--
error_reporting=2047
--FILE--
<?php
echo md5(var_export(explode("\1", "a". chr(1). "b". chr(0). "d" . chr(1) . "f" . chr(1). "1" . chr(1) . "d"), TRUE));
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
?>
--EXPECTF--
26d4e18734cb2582df5055e2175223df
bool(false)
bool(false)
bool(false)
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
bool(false)
array(1) {
  [0]=>
  string(3) "acb"
}
array(1) {
  [0]=>
  string(11) "otherstring"
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
