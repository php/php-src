--TEST--
Method cannot be readonly in trait alias
--FILE--
<?php

class Test {
    use T { foo as readonly; }
}

?>
--EXPECTF--
Fatal error: Cannot use the readonly modifier on a method in %s on line %d
