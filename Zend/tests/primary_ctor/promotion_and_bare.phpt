--TEST--
Primary constructors: explicit modifier promotes, bare parameter does not
--FILE--
<?php
class Foo(
    int $x,
    public int $y,
    private int $z = 9,
) {
    public function z(): int { return $this->z; }
}

$f = new Foo(1, 2);
var_dump(property_exists($f, 'x'));
var_dump(property_exists($f, 'y'));
var_dump(property_exists($f, 'z'));
var_dump($f->y, $f->z());

foreach ((new ReflectionMethod(Foo::class, '__construct'))->getParameters() as $p) {
    printf("%s promoted=%s\n", $p->getName(), $p->isPromoted() ? 'yes' : 'no');
}
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
int(2)
int(9)
x promoted=no
y promoted=yes
z promoted=yes
