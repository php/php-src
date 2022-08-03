--TEST--
Observer: fatal errors caught with zend_try will not fire end handlers prematurely
--EXTENSIONS--
zend_test
soap
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
function foo()
{
    // ext/soap catches a zend_bailout and then throws an exception
    $client = new SoapClient('foo');
}

function main()
{
    foo();
}

// try/catch is on main() to ensure ZEND_HANDLE_EXCEPTION does not fire the end handlers again
try {
    main();
} catch (SoapFault $e) {
    echo $e->getMessage() . PHP_EOL;
}

echo 'Done.' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s%eobserver_error_%d.php' -->
<file '%s%eobserver_error_%d.php'>
  <!-- init main() -->
  <main>
    <!-- init foo() -->
    <foo>
      <!-- init SoapClient::__construct() -->
      <SoapClient::__construct>
        <!-- Exception: SoapFault -->
      </SoapClient::__construct:NULL>
      <!-- Exception: SoapFault -->
    </foo:NULL>
    <!-- Exception: SoapFault -->
  </main:NULL>
  <!-- init Exception::getMessage() -->
  <Exception::getMessage>
  </Exception::getMessage:'SOAP-ERROR: Parsing WSDL: Couldn\'t load from \'foo\' : failed to load external entity "foo"
'>
SOAP-ERROR: Parsing WSDL: Couldn't load from 'foo' : failed to load external entity "foo"

Done.
</file '%s%eobserver_error_%d.php'>
