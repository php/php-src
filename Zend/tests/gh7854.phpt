--TEST--
Bug GH-7854 (Segfault on one-arm match statement)
--FILE--
<?php
[$a, $b] = match (true) {
    true => ['three', 'whatever'],
};
var_dump($a, $b);
?>
--EXPECT--
string(5) "three"
string(8) "whatever"
