--TEST--
Generic syntax: turbofish on new with various class-ref forms
--FILE--
<?php
class Box<T> {
    public function __construct(public int $v) {}
}

$b = new Box::<int>(1);
echo $b->v, "\n";

$cls = 'Box';
$b = new $cls::<int>(2);
echo $b->v, "\n";

$expr = fn() => 'Box';
$b = new ($expr())::<int>(3);
echo $b->v, "\n";

class Container<T> {
    public static function make(): self<int> {
        return new self::<int>;
    }
}
$c = Container::make();
echo get_class($c), "\n";
?>
--EXPECT--
1
2
3
Container
