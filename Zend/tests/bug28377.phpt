--TEST--
Bug #28377 (debug_backtrace is intermittently passing args)
--FILE--
<?php
function doit($a, $b)
{
  $trace = debug_backtrace();
  custom_callback('dereferenced', $trace);
  custom_callback('direct', debug_backtrace());
}
  
function custom_callback($traceName, $btInfo)
{
  echo $traceName ." -- args: ";
  echo isset($btInfo[0]['args']) ? count($btInfo[0]['args']) : 'does not exist';
  echo "\n";
}
  
doit('a','b');
?>
--EXPECT--
dereferenced -- args: 2
direct -- args: 2
