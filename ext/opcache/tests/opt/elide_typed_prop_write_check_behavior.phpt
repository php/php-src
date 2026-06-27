--TEST--
Typed property write check elision preserves runtime behavior (exact match, coercion, both modes)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
class C {
    public int $i = 0;
    public float $f = 0.0;
    public string $s = '';
    public bool $b = false;
    public ?int $ni = 0;
    public function __construct(int $i, string $s) { $this->i = $i; $this->s = $s; }
    public function setBool(bool $b): void { $this->b = $b; }
    public function setNullable(?int $ni): void { $this->ni = $ni; }
    public function coerceToFloat(int $x): void { $this->f = $x; }
}

$c = new C(5, "hello");
var_dump($c->i, $c->s);
$c->setBool(true);
var_dump($c->b);
$c->setNullable(null);
var_dump($c->ni);
$c->setNullable(7);
var_dump($c->ni);
$c->coerceToFloat(3);
var_dump($c->f);

class Promoted {
    public function __construct(public int $x, public readonly string $y) {}
}
$p = new Promoted(1, "ok");
var_dump($p->x, $p->y);
?>
--EXPECT--
int(5)
string(5) "hello"
bool(true)
NULL
int(7)
float(3)
int(1)
string(2) "ok"
