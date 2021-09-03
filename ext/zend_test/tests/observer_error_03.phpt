--TEST--
Observer: non-fatal errors do not fire end handlers prematurely
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
function foo()
{
    return $this_does_not_exit; // E_WARNING
}

function main()
{
    foo();
    echo 'After error.' . PHP_EOL;
}

main();

echo 'Done.' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s%eobserver_error_%d.php' -->
<file '%s%eobserver_error_%d.php'>
  <!-- init main() -->
  <main>
    <!-- init foo() -->
    <foo>

Warning: Undefined variable $this_does_not_exit in %s on line %d
    </foo:NULL>
After error.
  </main:NULL>
Done.
</file '%s%eobserver_error_%d.php'>
