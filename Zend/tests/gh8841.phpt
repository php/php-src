--TEST--
GH-8841 (php-cli core dump calling a badly formed function)
--FILE--
<?php
register_shutdown_function(function() {
    echo "Before calling g()\n";
    g(1);
    echo "After calling g()\n";
});

register_shutdown_function(function() {
    echo "Before calling f()\n";
    f(1);
    echo "After calling f()\n";
});

eval('function g($x): int { return $x; }');
eval('function f($x): void { return $x; }');
?>
--EXPECTF--
Fatal error: A void function must not return a value in %s on line %d
Before calling g()
After calling g()
Before calling f()

Fatal error: Uncaught Error: Call to undefined function f() in %s:%d
Stack trace:
#0 [internal function]: {closure}()
#1 {main}
  thrown in %s on line %d
