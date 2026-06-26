--TEST--
Inheritance: extending a base bound with an intersection of object-bounded params erases the inherited method to object
--FILE--
<?php
interface A {}
interface B {}
interface TI<out T> { public function get(): T; }
abstract class Base<out T> implements TI<T> {}

final class Inter<Tl: object, Tr: object> extends Base<Tl & Tr> {
    public function __construct(private TI<Tl> $l, private TI<Tr> $r) {}
    public function get(): Tl & Tr { return $this->l->get(); }
}

class AT implements TI<A> { public function get(): A { return new class implements A {}; } }
class BT implements TI<B> { public function get(): B { return new class implements B {}; } }

$i = new Inter::<A, B>(new AT(), new BT());
\var_dump($i->get() instanceof A);
echo "Inter::get returns: ", (string) (new ReflectionMethod(Inter::class, 'get'))->getReturnType(), "\n";

interface I {}
interface J {}
abstract class B2<out T> { public function get(): T { throw new \Exception; } }
final class M1<Tl: object, Tr: I> extends B2<Tl & Tr> {
    public function get(): Tl & Tr { throw new \Exception; }
}
final class M2<Tl: I, Tr: J> extends B2<Tl & Tr> {
    public function get(): Tl & Tr { throw new \Exception; }
}
echo "M1::get returns: ", (string) (new ReflectionMethod(M1::class, 'get'))->getReturnType(), "\n";
echo "M2::get returns: ", (string) (new ReflectionMethod(M2::class, 'get'))->getReturnType(), "\n";
echo "ok\n";
?>
--EXPECT--
bool(true)
Inter::get returns: object
M1::get returns: I
M2::get returns: I&J
ok
