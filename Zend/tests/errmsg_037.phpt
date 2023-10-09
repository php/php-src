--TEST--
errmsg: properties cannot be abstract
--FILE--
<?php

class test {
    abstract $var = 1;
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot use the abstract modifier on a property in %s on line %d
