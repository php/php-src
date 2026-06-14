--TEST--
Bug #33116 (crash when assigning class name to global variable in autoloader)
--FILE--
<?php
spl_autoload_register(function ($class) {
  $GLOBALS['include'][] = $class;
  eval("class DefClass{}");
});

$a = new DefClass;
print_r($a);
print_r($GLOBALS['include']);
?>
--EXPECT--
DefClass Object
(
)
Array
(
    [0] => DefClass
)
