--TEST--
Bug #19859 (__call() does not catch call_user_func_array() calls)
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 is needed'); ?>
--FILE--
<?php
class test
{
  function __call($method,$args)
  {
    print "test::__call invoked for method '$method'\n";
  }
}
$x = new test;
$x->fake(1);
call_user_func_array(array($x,'fake'),array(1));
call_user_func(array($x,'fake'),2);
?>
--EXPECT--
test::__call invoked for method 'fake'
test::__call invoked for method 'fake'
test::__call invoked for method 'fake'
