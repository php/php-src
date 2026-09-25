--TEST--
Observer: Function return values are modifiable by observers
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.show_return_value=1
zend_test.observer.observe_function_names=foo
zend_test.observer.observe_end_call_function_name=hook
--FILE--
<?php
function foo(string $pin): string {

    return 'original return value';
}

function hook(): string {
  return 'hook value';
}

$res = foo('some value');
var_dump($res);
echo 'Done' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s%eobserver_retval_alter_%d.php' -->
<!-- init foo() -->
<foo>
  <!-- init hook() -->
</foo:'hook value'>
<!-- init var_dump() -->
string(10) "hook value"
Done
