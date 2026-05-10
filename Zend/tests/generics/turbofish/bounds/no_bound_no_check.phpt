--TEST--
Turbofish bound check: parameters without bounds accept any type
--FILE--
<?php
class Box<T> {}
function id<T>(): void {}

new Box::<int>;
new Box::<string>;
id::<int>();
id::<string>();
echo "OK\n";
?>
--EXPECT--
OK
