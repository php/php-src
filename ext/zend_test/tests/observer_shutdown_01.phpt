--TEST--
Observer: Function calls from a shutdown handler are observable
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
register_shutdown_function(function () {
    echo 'Shutdown: ' . foo() . PHP_EOL;
});

function bar($arg) {
    return $arg;
}

function foo() {
    bar(41);
    return bar(42);
}

echo 'Done: ' . bar(40) . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init register_shutdown_function() -->
  <register_shutdown_function>
  </register_shutdown_function:NULL>
  <!-- init bar() -->
  <bar>
  </bar:40>
Done: 40
</file '%s'>
<!-- init {closure}() -->
<{closure}>
  <!-- init foo() -->
  <foo>
    <bar>
    </bar:41>
    <bar>
    </bar:42>
  </foo:42>
Shutdown: 42
</{closure}:NULL>
