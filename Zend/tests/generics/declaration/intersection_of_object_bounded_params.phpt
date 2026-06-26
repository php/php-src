--TEST--
Generics: an intersection of object-bounded type parameters erases to object (or collapses to a single class)
--FILE--
<?php
interface A {}
interface B {}
interface TI<out T> {}
abstract class Base<out T> implements TI<T> {}

final class Inter<Tl: object, Tr: object> extends Base<Tl & Tr> {
    public function get(): Tl & Tr {
        return new class implements A, B {};
    }
}

$i = new Inter::<A, B>();
\var_dump($i->get() instanceof A, $i->get() instanceof B);
echo "Inter::get returns: ", (string) (new ReflectionMethod(Inter::class, 'get'))->getReturnType(), "\n";

interface I {}
class C1<Tl: object, Tr: I> {
    public function g(): Tl & Tr { throw new \Exception; }
}
echo "C1::g returns: ", (string) (new ReflectionMethod(C1::class, 'g'))->getReturnType(), "\n";

interface J {}
class C2<Tl: I, Tr: J> {
    public function g(): Tl & Tr { throw new \Exception; }
}
echo "C2::g returns: ", (string) (new ReflectionMethod(C2::class, 'g'))->getReturnType(), "\n";

class C3<Tl: object, Tr: object> {
    public function g(): (Tl & Tr)|null { return null; }
}
echo "C3::g returns: ", (string) (new ReflectionMethod(C3::class, 'g'))->getReturnType(), "\n";

class C4<Tl: I, Tr: J> {
    public function g(): (Tl & Tr)|null { return null; }
}
echo "C4::g returns: ", (string) (new ReflectionMethod(C4::class, 'g'))->getReturnType(), "\n";
echo "ok\n";
?>
--EXPECT--
bool(true)
bool(true)
Inter::get returns: object
C1::g returns: I
C2::g returns: I&J
C3::g returns: ?object
C4::g returns: (I&J)|null
ok
