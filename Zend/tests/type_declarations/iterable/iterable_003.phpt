--TEST--
iterable type#003 - Return types
--FILE--
<?php

function foo(): iterable {
    return [];
}
function bar(): iterable {
    return (function () { yield; })();
}

function baz(): iterable {
    return 1;
}

var_dump(foo());
var_dump(bar());

try {
    baz();
} catch (Throwable $e) {
    echo $e->getMessage();
}

?>
--EXPECTF--
array(0) {
}
object(Generator)#%d (1) {
  ["function"]=>
  string(17) "{closure:bar():7}"
}
baz(): Return value must be of type Traversable|array, int returned
