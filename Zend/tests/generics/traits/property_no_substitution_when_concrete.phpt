--TEST--
Traits: property already concretely typed in the trait is unaffected by `use` clause arguments
--FILE--
<?php
trait Holder<T> {
    public int $count = 0;
    public T $val;
}

class StrBox { use Holder<string>; }

$rcCount = (new ReflectionClass('StrBox'))->getProperty('count');
$rcVal = (new ReflectionClass('StrBox'))->getProperty('val');
echo $rcCount->getType()->getName(), "/", $rcVal->getType()->getName(), "\n";
?>
--EXPECT--
int/string
