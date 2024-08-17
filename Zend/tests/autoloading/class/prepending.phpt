--TEST--
Prepending autoloaders
--FILE--
<?php
function autoloadA($name) {
    echo "A -> $name\n";
}
function autoloadB($name) {
    echo "B -> $name\n";
}
function autoloadC($name) {
    echo "C -> $name\n";
    class C{}
}

autoload_register_class('autoloadA');
autoload_register_class('autoloadB', true);
autoload_register_class('autoloadC');
var_dump(autoload_list_class());

new C;
?>
--EXPECT--
array(3) {
  [0]=>
  string(9) "autoloadB"
  [1]=>
  string(9) "autoloadA"
  [2]=>
  string(9) "autoloadC"
}
B -> C
A -> C
C -> C
