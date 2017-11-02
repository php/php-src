--TEST--
Bug #37676 (Function warnings)
--FILE--
<?php
function foo() {
    return null;
}

function foo_array() {
    return [null];
}

foo()[0];
foo()[0][1];
?>
--EXPECTF--
Warning: Variable of type null does not accept array offsets in %s on line %d

Warning: Variable of type null does not accept array offsets in %s on line %d
