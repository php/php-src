--TEST--
GH-10497: Passing constant object property by reference via FUNC_ARG
--FILE--
<?php

// Forward-reference: function declared after call site, so the compiler
// uses BP_VAR_FUNC_ARG rather than BP_VAR_W for the property fetch.
const OBJ = new stdClass;
OBJ->val = 10;
modify(OBJ->val);
var_dump(OBJ->val);

function modify(&$v) {
    $v = 42;
}

?>
--EXPECT--
int(42)
