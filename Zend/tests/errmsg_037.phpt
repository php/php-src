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
Fatal error: Properties cannot be declared abstract in %s on line %d
