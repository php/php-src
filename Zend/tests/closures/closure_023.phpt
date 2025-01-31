--TEST--
Closure 023: Closure declared in statically called method
--FILE--
<?php
class foo {
    public static function bar() {
        $func = function() { echo "Done"; };
        $func();
    }
}
foo::bar();
?>
--EXPECT--
Done
