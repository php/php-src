--TEST--
Testing Closure::fromCallable() functionality: Errors
--FILE--
<?php

include('closure_from_callable.inc');

echo 'Cannot access privateInstance method statically'."\n";
try {
    $fn = Closure::fromCallable(['Foo', 'privateInstanceFunc']);
    echo "Test failed to fail and return was : ".var_export($fn, true)."\n";
}
catch (\TypeError $te) {
    //This is the expected outcome.
}
catch (\Throwable $t) {
    echo "Wrong exception type thrown: ".get_class($t)." : ".$t->getMessage()."\n";
}


echo 'Cannot access privateInstance method statically with colon scheme'."\n";
try {
    $fn = Closure::fromCallable('Foo::privateInstanceFunc');
    echo "Test failed to fail and return was : ".var_export($fn, true)."\n";
}
catch (\TypeError $te) {
    //This is the expected outcome.
}
catch (\Throwable $t) {
    echo "Wrong exception type thrown: ".get_class($t)." : ".$t->getMessage()."\n";
}

echo 'Cannot access privateInstance method'."\n";
try {
    $fn = Closure::fromCallable([new Foo, 'privateInstanceFunc']);
    echo "Test failed to fail and return was : ".var_export($fn, true)."\n";
}
catch (\TypeError $te) {
    //This is the expected outcome.
}
catch (\Throwable $t) {
    echo "Wrong exception type thrown: ".get_class($t)." : ".$t->getMessage()."\n";
}

echo 'SubClass cannot access private instance method'."\n";
try {
    $fn = Closure::fromCallable([new SubFoo, 'privateInstanceFunc']);
    echo "Test failed to fail, closure is : ".var_export($fn, true)."\n";
}
catch (\TypeError $te) {
    //This is the expected outcome.
}
catch (\Throwable $t) {
    echo "Wrong exception type thrown: ".get_class($t)." : ".$t->getMessage()."\n";
}

echo 'Cannot access private static function of instance'."\n";
try {
    $fn = Closure::fromCallable([new Foo, 'privateStaticFunction']);
    echo "Test failed to fail, closure is : ".var_export($fn, true)."\n";
}
catch (\TypeError $te) {
    //This is the expected outcome.
}
catch (\Throwable $t) {
    echo "Wrong exception type thrown: ".get_class($t)." : ".$t->getMessage()."\n";
}

echo 'Cannot access private static method statically'."\n";
try {
    $fn = Closure::fromCallable(['Foo', 'privateStaticFunction']);
    echo "Test failed to fail, closure is : ".var_export($fn, true)."\n";
}
catch (\TypeError $te) {
    //This is the expected outcome.
}
catch (\Throwable $t) {
    echo "Wrong exception type thrown: ".get_class($t)." : ".$t->getMessage()."\n";
}

echo 'Cannot access private static method statically with colon scheme'."\n";
try {
    $fn = Closure::fromCallable('Foo::privateStaticFunction');
    echo "Test failed to fail, closure is : ".var_export($fn, true)."\n";
}
catch (\TypeError $te) {
    //This is the expected outcome.
}
catch (\Throwable $t) {
    echo "Wrong exception type thrown: ".get_class($t)." : ".$t->getMessage()."\n";
}

echo 'Non-existent method should fail'."\n";
try {
    $fn = Closure::fromCallable('Foo::nonExistentFunction');
    echo "Test failed to fail, closure is : ".var_export($fn, true)."\n";
}
catch (\TypeError $te) {
    //This is the expected outcome.
}
catch (\Throwable $t) {
    echo "Wrong exception type thrown: ".get_class($t)." : ".$t->getMessage()."\n";
}

echo 'Non-existent class should fail'."\n";
try {
    $fn = Closure::fromCallable(['NonExistentClass', 'foo']);
    echo "Test failed to fail, closure is : ".var_export($fn, true)."\n";
}
catch (\TypeError $te) {
    //This is the expected outcome.
}
catch (\Throwable $t) {
    echo "Wrong exception type thrown: ".get_class($t)." : ".$t->getMessage()."\n";
}

echo 'Non-existent function should fail'."\n";
try {
    $fn = Closure::fromCallable('thisDoesNotExist');
    echo "Test failed to fail, closure is : ".var_export($fn, true)."\n";
}
catch (\TypeError $te) {
    //This is the expected outcome.
}
catch (\Throwable $t) {
    echo "Wrong exception type thrown: ".get_class($t)." : ".$t->getMessage()."\n";
}


echo 'Subclass cannot closure over parent private instance method'."\n";
try {
    $subFoo = new SubFoo;
    $fn = $subFoo->closePrivateInvalid();
    echo "Test failed to fail, closure is : ".var_export($fn, true)."\n";
}
catch (\TypeError $te) {
    //This is the expected outcome.
}
catch (\Throwable $t) {
    echo "Wrong exception type thrown: ".get_class($t)." : ".$t->getMessage()."\n";
}

echo 'Subclass cannot closure over parant private static method'."\n";
try {
    $subFoo = new SubFoo;
    $fn = $subFoo->closePrivateStaticInvalid();
    echo "Test failed to fail, closure is : ".var_export($fn, true)."\n";
}
catch (\TypeError $te) {
    //This is the expected outcome.
}
catch (\Throwable $t) {
    echo "Wrong exception type thrown: ".get_class($t)." : ".$t->getMessage()."\n";
}

echo 'Function scope cannot closure over protected instance method'."\n";
try {
    $fn = functionAccessProtected();
    echo "Test failed to fail, closure is : ".var_export($fn, true)."\n";
}
catch (\TypeError $te) {
    //This is the expected outcome.
}
catch (\Throwable $t) {
    echo "Wrong exception type thrown: ".get_class($t)." : ".$t->getMessage()."\n";
}

echo 'Function scope cannot closure over private instance method'."\n";
try {
    $fn = functionAccessPrivate();
    echo "Test failed to fail, closure is : ".var_export($fn, true)."\n";
}
catch (\TypeError $te) {
    //This is the expected outcome.
}
catch (\Throwable $t) {
    echo "Wrong exception type thrown: ".get_class($t)." : ".$t->getMessage()."\n";
}

echo 'Access private instance method of parent object through "self::" to parent method'."\n";
try {
    $foo = new SubFoo;
    $fn = $foo->getSelfColonParentPrivateInstanceMethod();
    echo "Test failed to fail, closure is : ".var_export($fn, true)."\n";
}
catch (\TypeError $te) {
    //This is the expected outcome.
}
catch (\Throwable $t) {
    echo "Wrong exception type thrown: ".get_class($t)." : ".$t->getMessage()."\n";
}

echo "OK\n";

?>
--EXPECTF--
Cannot access privateInstance method statically
Cannot access privateInstance method statically with colon scheme
Cannot access privateInstance method
SubClass cannot access private instance method
Cannot access private static function of instance
Cannot access private static method statically
Cannot access private static method statically with colon scheme
Non-existent method should fail
Non-existent class should fail
Non-existent function should fail
Subclass cannot closure over parent private instance method
Subclass cannot closure over parant private static method
Function scope cannot closure over protected instance method
Function scope cannot closure over private instance method
Access private instance method of parent object through "self::" to parent method

Deprecated: Use of "self" in callables is deprecated in %s on line %d
OK
