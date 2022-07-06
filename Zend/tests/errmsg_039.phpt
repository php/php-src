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
Fatal error: Cannot redeclare test::$var in %s on line %d
