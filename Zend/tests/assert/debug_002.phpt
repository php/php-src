--TEST--
Debug statements should not run when assertions are disabled
--INI--
zend.assertions=0
assert.exception=0
--FILE--
<?php

    __debug {
        echo "Debugging mode is enabled, but it shouldn't be\n";
    }

    echo 'END';

?>
--EXPECT--
END
