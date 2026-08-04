--TEST--
Testing Closure::fromCallable() functionality: Basic
--FILE--
<?php

include('closure_from_callable.inc');

echo 'Access public static function';
$fn = Closure::fromCallable(['Foo', 'publicStaticFunction']);
echo $fn(" OK".PHP_EOL);

echo 'Access public static function with different case';
$fn = Closure::fromCallable(['fOo', 'publicStaticfUNCTION']);
echo $fn(" OK".PHP_EOL);

echo 'Access public static function with colon scheme';
$fn = Closure::fromCallable('Foo::publicStaticFunction');
echo $fn(" OK".PHP_EOL);

echo 'Access public instance method of object';
$fn = Closure::fromCallable([new Foo, 'publicInstanceFunc']);
echo $fn(" OK".PHP_EOL);

echo 'Access public instance method of parent object through parent:: ';
$fn = Closure::fromCallable([new Foo, 'publicInstanceFunc']);
echo $fn(" OK".PHP_EOL);

echo 'Function that exists';
$fn = Closure::fromCallable('bar');
echo $fn(" OK".PHP_EOL);

echo 'Function that exists with different spelling';
$fn = Closure::fromCallable('BAR');
echo $fn(" OK".PHP_EOL);

echo 'Closure is already a closure';
$fn = Closure::fromCallable($closure);
echo $fn(" OK".PHP_EOL);

echo 'Class with public invocable';
$fn = Closure::fromCallable(new PublicInvocable);
echo $fn(" OK".PHP_EOL);

echo "Instance return private method as callable";
$foo = new Foo;
$fn = $foo->closePrivateValid();
echo $fn(" OK".PHP_EOL);

echo "Instance return private static method as callable";
$foo = new Foo;
$fn = $foo->closePrivateStatic();
echo $fn(" OK".PHP_EOL);

echo 'Instance return protected static method as callable';
$subFoo = new SubFoo;
$fn = $subFoo->closeProtectedStaticMethod();
echo $fn(" OK".PHP_EOL);

echo 'Subclass closure over parent class protected method';
$subFoo = new SubFoo;
$fn = $subFoo->closeProtectedValid();
echo $fn(" OK".PHP_EOL);

echo 'Subclass closure over parent class static protected method';
$subFoo = new SubFoo;
$fn = $subFoo->closeProtectedStaticMethod();
echo $fn(" OK".PHP_EOL);

echo 'Access public instance method of parent object through "parent::" ';
$subFoo = new SubFoo;
$fn = $subFoo->getParentPublicInstanceMethod();
echo $fn(" OK".PHP_EOL);

echo 'Access public instance method of self object through "self::" ';
$foo = new Foo;
$fn = $foo->getSelfColonPublicInstanceMethod();
echo $fn(" OK".PHP_EOL);

echo 'Access public instance method of parent object through "self::" to parent method';
$foo = new SubFoo;
$fn = $foo->getSelfColonParentPublicInstanceMethod();
echo $fn(" OK".PHP_EOL);

echo 'Access protected instance method of parent object through "self::" to parent method';
$foo = new SubFoo;
$fn = $foo->getSelfColonParentProtectedInstanceMethod();
echo $fn(" OK".PHP_EOL);

echo 'MagicCall __call instance method ';
$fn = Closure::fromCallable([new MagicCall, 'nonExistentMethod']);
echo $fn(" OK".PHP_EOL);

echo 'MagicCall __callStatic static method ';
$fn = Closure::fromCallable(['MagicCall', 'nonExistentMethod']);
echo $fn(" OK".PHP_EOL);


?>
--EXPECTF--
Access public static function OK
Access public static function with different case
Fatal error: Uncaught TypeError: Failed to create closure from callable: class "fOo" not found in %s:%d
Stack trace:
#%d %s(10): Closure::fromCallable(Array)
#%d {main}
  thrown in %s on line %d
