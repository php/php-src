--TEST--
Scope function throws Error when stored in global
--FILE--
<?php
function make() {
    $fn = fn() { return 1; };
    $GLOBALS["escaped"] = $fn;
}
try {
    make();
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Scope function closure must not outlive the declaring scope
