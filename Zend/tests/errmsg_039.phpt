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
Fatal error: Property test::$var cannot be redeclared in %s on line %d
