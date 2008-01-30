--TEST--
Ensure callback methods in unknown classes trigger autoload.
--FILE--
<?php
  function __autoload($name)
  {
      echo "In autoload: ";
      var_dump($name);
  }
  call_user_func("UndefC::test");
?>
--EXPECTF--
In autoload: string(6) "UndefC"

Warning: call_user_func() expects parameter 1 to be valid callback, string given in %s on line %d

--UEXPECTF--
In autoload: unicode(6) "UndefC"

Warning: call_user_func() expects parameter 1 to be valid callback, Unicode string given in %s on line %d 
