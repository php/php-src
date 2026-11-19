--TEST--
Primary constructors: basic promotion, defaults, named args
--FILE--
<?php
class Point(public int $x, public int $y = 0) {
    public function sum(): int { return $this->x + $this->y; }
}

$p = new Point(3);
var_dump($p->x, $p->y, $p->sum());

$p2 = new Point(x: 10, y: 20);
var_dump($p2->sum());

$r = new ReflectionMethod(Point::class, '__construct');
var_dump($r->getNumberOfParameters());
var_dump($r->isPublic());
?>
--EXPECT--
int(3)
int(0)
int(3)
int(30)
int(2)
bool(true)
