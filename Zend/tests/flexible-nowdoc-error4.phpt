--TEST--
Flexible nowdoc syntax error 4: not enough body indentation
--FILE--
<?php

try {
    echo <<<'END'
      a
     b
    c
     END;
} catch (ParseError $pe) {
    var_dump($e->getMessage());
}
--EXPECTF--
Parse error: Invalid body indentation level (expecting an indentation at least 5) in %s on line %d