--TEST--
Bug #70547 (unsetting function variables corrupts backtrace)
--FILE--
<?php
function brokenTrace($arg1, &$arg2, $arg3){
	backtraceWrapper();
	var_dump(func_get_args());
	unset($arg3);
	var_dump(func_get_arg(0));
	var_dump(func_get_arg(1));
	var_dump(func_get_arg(2));
	var_dump(func_get_arg(3));
	backtraceWrapper();
	unset($arg1);
	var_dump(func_get_args());
	backtraceWrapper();
	unset($arg2);
	backtraceWrapper();
	var_dump(func_get_arg(0));
	var_dump(func_get_arg(1));
	var_dump(func_get_arg(2));
	var_dump(func_get_arg(3));
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
string(3) "1st"
string(3) "2nd"
NULL
string(3) "4th"
array(3) {
  [0]=>
  string(3) "1st"
  [1]=>
  &string(3) "2nd"
  [3]=>
  string(3) "4th"
}
array(2) {
  [1]=>
  string(3) "2nd"
  [3]=>
  string(3) "4th"
}
array(2) {
  [1]=>
  &string(3) "2nd"
  [3]=>
  string(3) "4th"
}
array(1) {
  [3]=>
  string(3) "4th"
}
NULL
NULL
NULL
string(3) "4th"
