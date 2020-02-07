--TEST--
Bug #35393 (changing static protected members from outside the class)
--INI--
error_reporting=4095
--FILE--
<?php
class ObjectPath
{
    static protected $type = array(0=>'main');

    static function getType()
    {
        return self::$type;
    }
}
print_r(ObjectPath::getType());
try {
	$object_type = array_pop((ObjectPath::getType()));
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
print_r(ObjectPath::getType());
?>
--EXPECT--
Array
(
    [0] => main
)
Exception: Cannot pass parameter 1 by reference
Array
(
    [0] => main
)
