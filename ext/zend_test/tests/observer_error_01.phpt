--TEST--
Observer: End handlers fire after a fatal error
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
memory_limit=2M
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
Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0
<!-- init '%s%eobserver_error_%d.php' -->
<file '%s%eobserver_error_%d.php'>
  <!-- init foo() -->
  <foo>

Fatal error: Allowed memory size of 2097152 bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
  </foo:NULL>
</file '%s%eobserver_error_%d.php'>
