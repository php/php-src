--TEST--
Bug #71841 (EG(error_zval) is not handled well)
--FILE--
<?php
$z = unserialize('O:1:"A":0:{}');
try {
    var_dump($z->e.=0);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(++$z->x);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($z->y++);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$y = array(PHP_INT_MAX => 0);
try {
    var_dump($y[] .= 0);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(++$y[]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($y[]++);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
The script tried to modify a property on an incomplete object. Please ensure that the class definition "A" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition
The script tried to modify a property on an incomplete object. Please ensure that the class definition "A" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition
The script tried to modify a property on an incomplete object. Please ensure that the class definition "A" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition
Cannot add element to the array as the next element is already occupied
Cannot add element to the array as the next element is already occupied
Cannot add element to the array as the next element is already occupied
