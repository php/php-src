--TEST--
Reflection Bug #36337 (ReflectionProperty fails to return correct visibility)
--FILE--
<?php

abstract class enum_ {
    protected $_values;

    public function __construct() {
        $property = new ReflectionProperty(get_class($this),'_values');
        var_dump($property->isProtected());
    }

}

final class myEnum extends enum_ {
    public $_values = array(
           0 => 'No value',
       );
}

$x = new myEnum();

echo "Done\n";
?>
--EXPECT--
bool(false)
Done
