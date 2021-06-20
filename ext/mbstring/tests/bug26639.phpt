--TEST--
Bug #26639 (mb_convert_variables() clutters variables beyond the references)
--EXTENSIONS--
mbstring
--FILE--
<?php
$a = "‚ ‚¢‚¤‚¦‚¨";
$b = $a;
mb_convert_variables("EUC-JP", "Shift_JIS", $b);
var_dump($a);
var_dump($b);
unset($a);
unset($b);

$a = "‚ ‚¢‚¤‚¦‚¨";
$b = &$a;
mb_convert_variables("EUC-JP", "Shift_JIS", $b);
var_dump($a);
var_dump($b);
unset($a);
unset($b);

$a = "‚ ‚¢‚¤‚¦‚¨";
$b = array($a);
$c = $b;
mb_convert_variables("EUC-JP", "Shift_JIS", $c);
var_dump($b);
var_dump($c);
unset($a);
unset($b);
unset($c);

$a = "‚ ‚¢‚¤‚¦‚¨";
$b = array(&$a);
$c = $b;
mb_convert_variables("EUC-JP", "Shift_JIS", $c);
var_dump($b);
var_dump($c);
unset($a);
unset($b);
unset($c);

$a = "‚ ‚¢‚¤‚¦‚¨";
$b = array($a);
$c = &$b;
mb_convert_variables("EUC-JP", "Shift_JIS", $c);
var_dump($b);
var_dump($c);
unset($a);
unset($b);
unset($c);

$a = "‚ ‚¢‚¤‚¦‚¨";
$b = array(&$a);
$c = &$b;
mb_convert_variables("EUC-JP", "Shift_JIS", $c);
var_dump($b);
var_dump($c);
unset($a);
unset($b);
unset($c);

$a = array(array("‚ ‚¢‚¤‚¦‚¨"));
$b = $a;
$c = $b;
mb_convert_variables("EUC-JP", "Shift_JIS", $c);
var_dump($b);
var_dump($c);
unset($a);
unset($b);
unset($c);
?>
--EXPECT--
string(10) "‚ ‚¢‚¤‚¦‚¨"
string(10) "¤¢¤¤¤¦¤¨¤ª"
string(10) "¤¢¤¤¤¦¤¨¤ª"
string(10) "¤¢¤¤¤¦¤¨¤ª"
array(1) {
  [0]=>
  string(10) "‚ ‚¢‚¤‚¦‚¨"
}
array(1) {
  [0]=>
  string(10) "¤¢¤¤¤¦¤¨¤ª"
}
array(1) {
  [0]=>
  &string(10) "‚ ‚¢‚¤‚¦‚¨"
}
array(1) {
  [0]=>
  string(10) "¤¢¤¤¤¦¤¨¤ª"
}
array(1) {
  [0]=>
  string(10) "¤¢¤¤¤¦¤¨¤ª"
}
array(1) {
  [0]=>
  string(10) "¤¢¤¤¤¦¤¨¤ª"
}
array(1) {
  [0]=>
  string(10) "¤¢¤¤¤¦¤¨¤ª"
}
array(1) {
  [0]=>
  string(10) "¤¢¤¤¤¦¤¨¤ª"
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    string(10) "‚ ‚¢‚¤‚¦‚¨"
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    string(10) "¤¢¤¤¤¦¤¨¤ª"
  }
}
