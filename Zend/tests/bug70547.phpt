--TEST--
Bug #70547 (unsetting function variables corrupts backtrace)
--FILE--
<?php
function brokenTrace($arg1, &$arg2, $arg3){
	backtraceWrapper();
	var_dump(func_get_args());
	unset($arg3);
	backtraceWrapper();
	var_dump(func_get_args());
	unset($arg1);
	backtraceWrapper();
	var_dump(func_get_args());
	unset($arg2);
	backtraceWrapper();
	var_dump(func_get_args());
}
$arg2 = "2nd";
brokenTrace("1st", $arg2, "3th", "4th");
function backtraceWrapper(){
	$bt = debug_backtrace();
	var_dump($bt[1]['args']);
}
?>
--EXPECT--
array(4) {
  [0]=>
  string(3) "1st"
  [1]=>
  &string(3) "2nd"
  [2]=>
  string(3) "3th"
  [3]=>
  string(3) "4th"
}
array(4) {
  [0]=>
  string(3) "1st"
  [1]=>
  string(3) "2nd"
  [2]=>
  string(3) "3th"
  [3]=>
  string(3) "4th"
}
array(3) {
  [0]=>
  string(3) "1st"
  [1]=>
  &string(3) "2nd"
  [2]=>
  string(3) "4th"
}
array(3) {
  [0]=>
  string(3) "1st"
  [1]=>
  string(3) "2nd"
  [2]=>
  string(3) "4th"
}
array(2) {
  [0]=>
  &string(3) "2nd"
  [1]=>
  string(3) "4th"
}
array(2) {
  [0]=>
  string(3) "2nd"
  [1]=>
  string(3) "4th"
}
array(1) {
  [0]=>
  string(3) "4th"
}
array(1) {
  [0]=>
  string(3) "4th"
}
