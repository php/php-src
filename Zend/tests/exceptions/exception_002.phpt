--TEST--
Testing exception and GOTO
--FILE--
<?php

goto foo;

try {
    print 1;

    foo:
    print 2;
} catch (Exception $e) {

}

?>
--EXPECT--
2
