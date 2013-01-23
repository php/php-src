--TEST--
Bug #33558 (warning with nested calls to functions returning by reference)
--INI--
error_reporting=4095
--FILE--
<?php
function & foo() {
    $var = 'ok';
    return $var;
}

function & bar() {
    return foo();
}

$a =& bar();
echo "$a\n";
?>
--EXPECT--
ok

