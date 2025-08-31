--TEST--
ReflectionClass::getConstructor()
--FILE--
<?php
class NewCtor {
    function __construct() {}
}

class ExtendsNewCtor extends NewCtor {
}

$classes = array('NewCtor', 'ExtendsNewCtor');
foreach ($classes as $class) {
    $rc = new ReflectionClass($class);
    $rm = $rc->getConstructor();
    if ($rm != null) {
        echo "Constructor of $class: " . $rm->getName() . "\n";
    }  else {
        echo "No constructor for $class\n";
    }

}

?>
--EXPECT--
Constructor of NewCtor: __construct
Constructor of ExtendsNewCtor: __construct
