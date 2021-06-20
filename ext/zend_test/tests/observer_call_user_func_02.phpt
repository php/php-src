--TEST--
Observer: call_user_func_array() from root namespace
--EXTENSIONS--
zend_test
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
<!-- init '%s%eobserver_call_user_func_%d.php' -->
<file '%s%eobserver_call_user_func_%d.php'>
  <!-- init Test\MyClass::myMethod() -->
  <Test\MyClass::myMethod>
MyClass::myMethod called
  </Test\MyClass::myMethod>
  <!-- init Test\my_function() -->
  <Test\my_function>
my_function called
  </Test\my_function>
</file '%s%eobserver_call_user_func_%d.php'>
