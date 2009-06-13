--TEST--
SPL: spl_autoload_register() Bug #48541: registering multiple closures fails with memleaks
--FILE--
<?php
$a = function ($class) {
    echo "a called\n";
};
$b = function ($class) {
    eval('class ' . $class . '{function __construct(){echo "foo\n";}}');
    echo "b called\n";
};
spl_autoload_register($a);
spl_autoload_register($b);

$c = $a;
spl_autoload_register($c);
$c = new foo;
?>
===DONE===
--EXPECT--
a called
b called
foo
===DONE===