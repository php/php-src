--TEST--
errmsg: cannot redeclare property
--FILE--
<?php

class test {
    var $var;
    var $var;
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Property test::$var has already been declared in %s on line %d
