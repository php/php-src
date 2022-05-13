--TEST--
Method cannot be readonly in trait alias
--FILE--
<?php

class Test {
    use T { foo as readonly; }
}

?>
--EXPECTF--
Fatal error: Cannot use 'readonly' as method modifier in %s on line %d
