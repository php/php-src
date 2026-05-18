--TEST--
Errors: optional type parameters at the tail are accepted
--FILE--
<?php
class A<T> {}
class B<T = int> {}
class C<T, U = int> {}
class D<T, U = int, V = string> {}
class E<T = int, U = string> {}
function f<T, U = int>(): void {}
function g<T = int, U = string>(): void {}
echo "ok\n";
?>
--EXPECT--
ok
