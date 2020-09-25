--TEST--
Observer: fatal errors caught with zend_try will fire end handlers once
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
<?php if (!extension_loaded('soap')) die('skip: soap extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
function foo()
{
    try {
        // ext/soap catches a fatal error and then throws an exception
        $client = new SoapClient('foo');
    } catch (SoapFault $e) {
        echo $e->getMessage() . PHP_EOL;
    }
}

function main()
{
    foo();
}

main();

echo 'Done.' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s/observer_error_%d.php' -->
<file '%s/observer_error_%d.php'>
  <!-- init main() -->
  <main>
    <!-- init foo() -->
    <foo>
    </foo:NULL>
  </main:NULL>
</file '%s/observer_error_%d.php'>
SOAP-ERROR: Parsing WSDL: Couldn't load from 'foo' : failed to load external entity "foo"

Done.
