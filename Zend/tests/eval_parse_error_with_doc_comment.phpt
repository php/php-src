--TEST--
eval() parse error on function with doc comment
--FILE--
<?php

eval(
<<<EOC
/** doc comment */
function f() {
EOC
);

?>
--EXPECTF--
Parse error: Unclosed '{' in %s(%d) : eval()'d code on line %d
