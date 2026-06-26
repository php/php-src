--TEST--
Reflection: hasDefault() and getDefault()
--FILE--
<?php
class A<X, Y = int> {}
$ps = (new ReflectionClass('A'))->getGenericParameters();
var_dump($ps[0]->hasDefault());
try {
    $ps[0]->getDefault();
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($ps[1]->hasDefault());
echo $ps[1]->getDefault()->getName(), "\n";
?>
--EXPECT--
bool(false)
Type parameter X has no default
bool(true)
int
