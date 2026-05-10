--TEST--
Generics: a type parameter whose bound is not object-shaped is still rejected in an intersection
--FILE--
<?php
class C<Tl: int, Tr: object> {
    public function g(): Tl & Tr { throw new \Exception; }
}
?>
--EXPECTF--
Fatal error: Type parameter Tl with bound int cannot be part of an intersection type; use an object-shaped bound (e.g. Tl: object) in %s on line %d
