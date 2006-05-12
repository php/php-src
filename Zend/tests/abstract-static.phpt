--TEST--
Test for abstract static classes
--FILE--
<?php
abstract class ezcDbHandler extends PDO
{
    public function __construct( $dbParams, $dsn )
    {
        $user          = null;
        $pass          = null;
        $driverOptions = null;
    }

    abstract static public function getName();

    static public function hasFeature( $feature )
    {
        return false;
    }
}
?>
DONE
--EXPECT--
DONE
