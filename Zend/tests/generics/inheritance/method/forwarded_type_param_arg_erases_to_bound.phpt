--TEST--
Inheritance: a type parameter forwarded as a type argument in extends erases to its own bound
--FILE--
<?php
interface Acc<Tk: string|int, Tv> {
    public function ls(Tv $v): void;
}

interface CConcrete extends Acc<int, string> {
    public function ls(string $v): void;
}

interface CGeneric<T: string|int> extends Acc<T, T> {
    public function ls(T $v): void;
}

interface CForward<U: string|int> extends CGeneric<U> {
    public function ls(U $v): void;
}

echo "ok\n";

class Impl implements CGeneric<int> {
    public function ls(string|int $v): void { echo "ls(", get_debug_type($v), ")\n"; }
}
(new Impl)->ls(1);
(new Impl)->ls("x");
?>
--EXPECT--
ok
ls(int)
ls(string)
