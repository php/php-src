--TEST--
public(set) of trait property is properly propagated
--FILE--
<?php

trait T {
    public public(set) readonly int $pub;
    protected readonly int $prot;
    private readonly int $priv;

    public function modify($prop) {
        $this->{$prop} = 42;
        try {
            $this->{$prop} = 43;
        } catch (\Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
        var_dump($this->{$prop});
    }
}

class C {
    use T;
}

$c = new C;

$c->pub = 42;
try {
    $c->pub = 43;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($c->pub);

$c->modify('prot');
$c->modify('priv');

?>
--EXPECT--
Error: Cannot modify readonly property C::$pub
int(42)
Error: Cannot modify readonly property C::$prot
int(42)
Error: Cannot modify readonly property C::$priv
int(42)
