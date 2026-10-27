--TEST--
Observer: End handlers fire after a fatal error
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
function foo()
{
    var_dump(new class { public function __debugInfo() { return ''; } }); // cause a bailout
}

foo();

echo 'You should not see this.';
?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init foo() -->
  <foo>
    <!-- init var_dump() -->
    <var_dump>
      <!-- init class@anonymous::__debugInfo() -->
      <class@anonymous::__debugInfo>
      </class@anonymous::__debugInfo:''>

Fatal error: __debugInfo() must return an array in %s on line %d
    </var_dump:NULL>
  </foo:NULL>
</file '%s%eobserver_error_%d.php'>
