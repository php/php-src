--TEST--
Check call to non-ref function with call-time refs
--FILE--
<?php
function my_errorhandler($errno,$errormsg) {
  global $my_var;
  $my_var=0x12345;
  echo $errormsg."\n";
  return true;
}
$oldhandler = set_error_handler("my_errorhandler");
$my_var = str_repeat("A",64);
$data = call_user_func_array("substr_replace",array(&$my_var, new StdClass(),1));
echo "OK!";
--EXPECT--
Object of class stdClass could not be converted to string
OK!
