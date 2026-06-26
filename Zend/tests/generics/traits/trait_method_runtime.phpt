--TEST--
Traits: methods from trait work at runtime
--FILE--
<?php
class A {}
trait Holder<T : A> {
    public function get(T $x): T { return $x; }
}
class Box {
    use Holder<A>;
}

$a = new A;
$b = new Box;
echo get_class($b->get($a)), "\n";

try {
    $b->get("string");
    echo "no error\n";
} catch (TypeError $e) {
    echo "type error\n";
}
?>
--EXPECT--
A
type error
