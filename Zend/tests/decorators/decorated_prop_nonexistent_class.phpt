--TEST--
Decorated property references non-existing class
--FILE--
<?php

class Test {
    public decorated DoesntExist $prop;
}

?>
--EXPECTF--
Fatal error: Class 'DoesntExist' not found in %s on line %d
