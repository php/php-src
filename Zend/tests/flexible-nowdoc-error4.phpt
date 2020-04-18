--TEST--
Flexible nowdoc syntax error 4: not enough body indentation
--FILE--
<?php

echo <<<'END'
      a
     b
    c
     END;

?>
--EXPECTF--
Parse error: Invalid body indentation level (expecting an indentation level of at least 5) in %s on line %d
