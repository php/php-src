--TEST--
Coalesce block
--FILE--
<?php
$x = null;
$y = $x ?? {
    echo "Executed\n";
    42
};
var_dump($y);

$x = 42;
$y = $x ?? {
    echo "Never executed\n";
};
var_dump($y);
?>
--EXPECT--
Executed
int(42)
int(42)
