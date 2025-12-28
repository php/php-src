--TEST--
GH-7771.3 (Incorrect file/line for class constant expression exceptions)
--FILE--
<?php
$classlist = [
    'space1\C' => 'class y{const y="$y";}',
    'D' => 'class D{const HW=space1\C::y;}'
];
spl_autoload_register(function($class) use ($classlist) {
    eval($classlist[$class]);
});
var_dump(D::HW);
?>
--EXPECTF--
Fatal error: Constant expression contains invalid operations in %sgh7771_3.php(7) : eval()'d code on line 1

