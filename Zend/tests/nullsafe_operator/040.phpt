--TEST--
Memory leak in JMP_NULL
--FILE--
<?php
function &returns_ref($unused) {
    global $foo;
    return $foo;
}

function &returns_ref2() {
    return returns_ref(returns_ref(null)?->null);
}

$foo2 = &returns_ref2();
$foo2 = 'foo';
var_dump($foo);
?>
--EXPECT--
string(3) "foo"
