--TEST--
Traits: trait class constant with type parameter
--FILE--
<?php
trait Holder<T : int> {
    const T MIN = 0;
}
class Box {
    use Holder<int>;
}
$rc = new ReflectionClass('Box');
$consts = $rc->getReflectionConstants();
foreach ($consts as $c) {
    echo $c->getName(), ": ", $c->getValue(), "\n";
}
?>
--EXPECT--
MIN: 0
