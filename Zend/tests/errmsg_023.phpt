--TEST--
errmsg: access level must be the same or weaker
--FILE--
<?php

class test1 {
    protected $var;
}

class test extends test1 {
    private $var;
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Property test::$var must have protected or public visibility to be compatible with overridden property test1::$var in %s on line %d
