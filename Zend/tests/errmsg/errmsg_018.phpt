--TEST--
errmsg: static abstract function
--FILE--
<?php

class test {
    static abstract function foo ();
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Class test declares abstract method foo() and must therefore be declared abstract in %s on line %d
