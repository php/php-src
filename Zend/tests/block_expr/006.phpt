--TEST--
Parse error when semicolon after block expression is missing
--FILE--
<?php

function foo() {
    {
        echo "Foo\n";
        echo "Bar\n";
        'Baz'
    }
}

?>
--EXPECTF--
Parse error: syntax error, unexpected '}' in %s on line %d
