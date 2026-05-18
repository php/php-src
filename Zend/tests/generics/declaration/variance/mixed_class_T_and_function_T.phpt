--TEST--
Variance: class-T and function-T variance markers are checked independently in the same method
--FILE--
<?php
class C<+T> {
    public function map<-A, +B>(A $a): B { return null; }
}

(new C)->map(42);
echo "ok\n";
?>
--EXPECT--
ok
