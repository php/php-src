--TEST--
Scope function closure: $this rebind to a different object is rejected
--FILE--
<?php
function test() {
    /* No $this initially. Binding any $this is forbidden because the
     * existing this_ptr is undef — the new value is necessarily different. */
    $fn = fn() { return 1; };
    try {
        Closure::bind($fn, new stdClass);
        echo "no error\n";
    } catch (Error $e) {
        echo $e->getMessage() . "\n";
    }
}
test();
?>
--EXPECT--
Cannot rebind $this of a scope function
