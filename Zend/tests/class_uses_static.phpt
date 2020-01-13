--TEST--
Class cannot use static as a trait, as it is reserved
--FILE--
<?php

class Test {
    use static;
}

?>
--EXPECTF--
Fatal error: Cannot use 'static' as trait name, as it is reserved in %s on line %d
