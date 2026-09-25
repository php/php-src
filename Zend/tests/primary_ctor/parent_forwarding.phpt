--TEST--
Primary constructors: forward arguments to the parent constructor via extends
--FILE--
<?php
abstract class Shape(protected string $name) {
    abstract public function area(): float;
    public function name(): string { return $this->name; }
}

final class Circle(
    public readonly float $radius,
    string $name = 'circle',
) extends Shape($name) {
    public function area(): float { return $this->radius ** 2 * 3; }
}

$c = new Circle(2.0);
printf("%s %.1f\n", $c->name(), $c->area());

$c2 = new Circle(1.0, 'unit');
echo $c2->name(), "\n";

$rp = (new ReflectionMethod(Circle::class, '__construct'))->getParameters();
printf("radius promoted=%s, name promoted=%s\n",
    $rp[0]->isPromoted() ? 'yes' : 'no',
    $rp[1]->isPromoted() ? 'yes' : 'no');
?>
--EXPECT--
circle 12.0
unit
radius promoted=yes, name promoted=no
