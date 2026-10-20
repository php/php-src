--TEST--
GH-23679: tracing JIT must not write a parent private property into a child's shadowing public property
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
opcache.jit=tracing
opcache.jit_hot_loop=1
opcache.jit_hot_func=1
opcache.jit_hot_return=1
opcache.jit_hot_side_exit=1
--EXTENSIONS--
opcache
--FILE--
<?php
class A
{
    private $arr = [];
    private $n = 0;

    public function setDim($k, $v)
    {
        $x = clone $this;
        $x->arr[$k] = $v;
        return $x;
    }

    public function setAll($v)
    {
        $x = clone $this;
        $x->arr = $v;
        return $x;
    }

    public function inc()
    {
        $x = clone $this;
        $x->n++;
        return $x;
    }

    public function addN($k)
    {
        $x = clone $this;
        $x->n += $k;
        return $x;
    }

    public function getDim($k)
    {
        return $this->arr[$k] ?? 'MISSING';
    }

    public function getN()
    {
        return $this->n;
    }
}

class B extends A
{
    public $arr = [];
    public $n = 100;
}

for ($i = 0; $i < 50; $i++) {
    (new A)->setDim('x', 1)->getDim('x');
    (new A)->setAll(['x' => 1])->getDim('x');
    (new A)->inc()->getN();
    (new A)->addN(5)->getN();

    $b = new B;
    $b->arr = ['keep' => 1];
    $b->n = 100;

    $r = $b->setDim('x', 2);
    if ($r->getDim('x') !== 2 || $r->arr !== ['keep' => 1]) {
        echo "dim-assign i=$i private=";
        var_dump($r->getDim('x'));
        echo "dim-assign i=$i public=";
        var_dump($r->arr);
        exit(1);
    }

    $r = $b->setAll(['y' => 4]);
    if ($r->getDim('y') !== 4 || $r->arr !== ['keep' => 1]) {
        echo "assign i=$i private=";
        var_dump($r->getDim('y'));
        echo "assign i=$i public=";
        var_dump($r->arr);
        exit(1);
    }

    $r = $b->inc();
    if ($r->getN() !== 1 || $r->n !== 100) {
        echo "inc i=$i private=";
        var_dump($r->getN());
        echo "inc i=$i public=";
        var_dump($r->n);
        exit(1);
    }

    $r = $b->addN(5);
    if ($r->getN() !== 5 || $r->n !== 100) {
        echo "assign-op i=$i private=";
        var_dump($r->getN());
        echo "assign-op i=$i public=";
        var_dump($r->n);
        exit(1);
    }
}
echo "ok\n";
?>
--EXPECT--
ok
