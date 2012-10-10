--TEST--
Crash when function parameter modified via unexpected reference
--FILE--
<?php
function my_errorhandler($errno,$errormsg) {
  global $my_var;
  $my_var = 0;
  return true;
}
set_error_handler("my_errorhandler");
$my_var = str_repeat("A",64);
$data = call_user_func_array("explode",array(new StdClass(), &$my_var));
$my_var=array(1,2,3);
$data = call_user_func_array("implode",array(&$my_var, new StdClass()));
echo "Done.\n";
?>
--EXPECTF--
Done.
