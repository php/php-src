--TEST--
list<T> cannot be part of a union type
--FILE--
<?php
class User {}

function f(list<User>|null $users): void {}
?>
--EXPECTF--
Fatal error: Type list<T> cannot be part of a union type in %s on line %d
