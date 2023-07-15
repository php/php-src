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
    function foo(){}
}

autoload_register_function('autoloadA');
autoload_register_function('autoloadB', true);
autoload_register_function('autoloadC');
var_dump(autoload_list_function());

foo();
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
B -> foo
A -> foo
C -> foo
