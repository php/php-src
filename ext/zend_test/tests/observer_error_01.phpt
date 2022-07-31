--TEST--
Observer: End handlers fire after a fatal error
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
memory_limit=2M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip requires zmm");
?>
--FILE--
<?php
function foo()
{
    str_repeat('.', 1024 * 1024 * 2); // 2MB
}

foo();

echo 'You should not see this.';
?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init foo() -->
  <foo>
    <!-- init str_repeat() -->
    <str_repeat>

Fatal error: Allowed memory size of 2097152 bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
    </str_repeat:NULL>
  </foo:NULL>
</file '%s%eobserver_error_%d.php'>
