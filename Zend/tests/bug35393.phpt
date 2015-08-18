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
$object_type = array_pop((ObjectPath::getType()));
print_r(ObjectPath::getType());
?>
--EXPECTF--
Array
(
    [0] => main
)

Notice: Only variables should be passed by reference in %sbug35393.php on line 12
Array
(
    [0] => main
)
