--TEST--
Property substitution: parent property already typed with a concrete type is unchanged
--FILE--
<?php
class Box<T> {
    public int $count = 0;
    public T $val;
}

class StrBox extends Box<string> {}

$rcCount = (new ReflectionClass('StrBox'))->getProperty('count');
$rcVal = (new ReflectionClass('StrBox'))->getProperty('val');
echo $rcCount->getType()->getName(), "/", $rcVal->getType()->getName(), "\n";
?>
--EXPECT--
int/string
