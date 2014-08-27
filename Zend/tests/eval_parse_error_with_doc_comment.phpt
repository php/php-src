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
Parse error: syntax error, unexpected end of file in %s(%d) : eval()'d code on line %d
