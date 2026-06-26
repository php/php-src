--TEST--
Scope function throws Error when escaping the declaring scope
--FILE--
<?php
function make() {
    return fn() { return 1; };
}
try {
    $escaped = make();
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Scope function closure must not outlive the declaring scope
