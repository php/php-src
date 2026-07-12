--TEST--
GH-20477 (Use-after-free in concat when a destructor reads the concatenated value)
--FILE--
<?php
class Foo {
    public function __destruct() {
        global $a;
        serialize($a);
    }
}
$a = ['index.php', 'c' => new Foo];
$a .= 'x';
var_dump($a);
?>
--EXPECTF--
Warning: Array to string conversion in %s on line %d
string(6) "Arrayx"
