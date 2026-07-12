--TEST--
Observer: End handlers fire after a fatal error
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
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
      <!-- Exception: MemoryError -->
    </str_repeat:NULL>
    <!-- Exception: MemoryError -->
  </foo:NULL>
  <!-- Exception: MemoryError -->
</file '%s%eobserver_error_%d.php'>
<!-- init Error::__toString() -->
<Error::__toString>
  <!-- init Error::getTraceAsString() -->
  <Error::getTraceAsString>
  </Error::getTraceAsString:'#0 %s(%d): str_repeat(\'.\', 2097152)
#1 %s(%d): foo()
#2 {main}'>
</Error::__toString:'MemoryError: The resulting string is too large to fit in the configured memory limit in %s:%d
Stack trace:
#0 %s(%d): str_repeat(\'.\', 2097152)
#1 %s(%d): foo()
#2 {main}'>

Fatal error: Uncaught MemoryError: The resulting string is too large to fit in the configured memory limit in %s:%d
Stack trace:
#0 %s(%d): str_repeat('.', 2097152)
#1 %s(%d): foo()
#2 {main}
  thrown in %s on line %d
