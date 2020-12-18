--TEST--
Observer: call_user_func_array() from root namespace
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
--FILE--
<?php
namespace Test {
    final class MyClass
    {
        public static function myMethod(string $msg)
        {
            echo 'MyClass::myMethod ' . $msg . PHP_EOL;
        }
    }

    function my_function(string $msg)
    {
        echo 'my_function ' . $msg . PHP_EOL;
    }
}
namespace {
    call_user_func_array('Test\\MyClass::myMethod', ['called']);
    call_user_func_array('Test\\my_function', ['called']);
}
?>
--EXPECTF--
<!-- init '%s/observer_call_user_func_%d.php' -->
<file '%s/observer_call_user_func_%d.php'>
  <!-- init Test\MyClass::myMethod() -->
  <Test\MyClass::myMethod>
MyClass::myMethod called
  </Test\MyClass::myMethod>
  <!-- init Test\my_function() -->
  <Test\my_function>
my_function called
  </Test\my_function>
</file '%s/observer_call_user_func_%d.php'>
