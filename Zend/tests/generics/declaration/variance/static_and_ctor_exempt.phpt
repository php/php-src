--TEST--
Variance: static methods and constructors are exempt from variance position checks
--FILE--
<?php
class Pair<out A, in B> {
    private mixed $a;
    private mixed $b;

    public function __construct(A $a, B $b) {
        $this->a = $a;
        $this->b = $b;
    }

    public static function of(A $a, B $b): static {
        return new static($a, $b);
    }

    public static function echoB(B $b): B {
        return $b;
    }

    public function dump(): void {
        \var_dump($this->a, $this->b);
    }
}

$p = Pair::of(1, "x");
$p->dump();
\var_dump(Pair::echoB("y"));
echo "ok\n";
?>
--EXPECT--
int(1)
string(1) "x"
string(1) "y"
ok
