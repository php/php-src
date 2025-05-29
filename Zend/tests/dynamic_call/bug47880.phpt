--TEST--
Bug #47880 (crashes in call_user_func_array())
--FILE--
<?php
class bomb {
  static function go($n)	{
   $backtrace = debug_backtrace(false);
   $backtrace[1]['args'][1] = 'bomb';
  }
}
call_user_func_array(array('bomb', 'go'), array(0));
echo "ok\n";
?>
--EXPECT--
ok
