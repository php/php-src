--TEST--
Debug statements should only run when assertions are enabled
--INI--
zend.assertions=1
assert.exception=0
--FILE--
<?php

    __debug printf("Debugging mode is enabled (Expr)\n");

    __debug {
        printf("Debugging mode is enabled (S1)\n");
        printf("Debugging mode is enabled (S2)\n");
    }

    echo 'END';

?>
--EXPECT--
Debugging mode is enabled (Expr)
Debugging mode is enabled (S1)
Debugging mode is enabled (S2)
END
