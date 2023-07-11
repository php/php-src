--TEST--
Combining property capture with custom methods
--FILE--
<?php
$a = 1; $b = 2; $c = 3; $d = 4; $e = 5; $f = 6; $g = 7; $h = 8;

$foo = new class use (
    $a,
    $b as $b_name,
    $c as private,
    $d as private $d_name,
    $e as int,
    $f as int $f_name,
    $g as private int,
    $h as private int $h_name,
) {
    public function getA() { return $this->a; }
    public function getB() { return $this->b_name; }
    public function getC() { return $this->c; }
    public function getD() { return $this->d_name; }
    public function getE() { return $this->e; }
    public function getF() { return $this->f_name; }
    public function getG() { return $this->g; }
    public function getH() { return $this->h_name; }
};

var_dump(
    $foo->getA(), $foo->getB(), $foo->getC(), $foo->getD(), $foo->getE(), $foo->getF(), $foo->getG(), $foo->getH(),
);
?>
--EXPECT--
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(7)
int(8)
