--TEST--
Suppression operator ParseError with empty class list
--FILE--
<?php

function test1() {
    throw new Exception();
    return true;
}

$var = @<>test1();

var_dump($var);

echo "Done\n";
?>
--EXPECTF--
Parse error: syntax error, unexpected token ">" in %s on line 8
