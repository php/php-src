--TEST--
getAttribute() with ReflectionAttribute::IS_INSTANCEOF flag
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL | Attribute::IS_REPEATABLE)]
class Base {}

#[Attribute(Attribute::TARGET_ALL | Attribute::IS_REPEATABLE)]
class Child extends Base {}

#[Child, Base]
function myFunc() {}

$rf = new ReflectionFunction('myFunc');

// IS_INSTANCEOF returns first subclass match
var_dump($rf->getAttribute('Base', ReflectionAttribute::IS_INSTANCEOF)?->getName());

// IS_INSTANCEOF also matches the class itself
var_dump($rf->getAttribute('Child', ReflectionAttribute::IS_INSTANCEOF)?->getName());

// Invalid flags raise an error
try {
    $rf->getAttribute(null, 99);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
string(5) "Child"
string(5) "Child"
ReflectionFunctionAbstract::getAttribute(): Argument #2 ($flags) must be a valid attribute filter flag
