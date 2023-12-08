--TEST--
Coalesce assignment block
--FILE--
<?php
$x = null;
$x ??= {
    echo "Executed\n";
    42
};
var_dump($x);

$x = 42;
$x ??= {
    echo "Never executed\n";
};
var_dump($x);
?>
--EXPECT--
Executed
int(42)
int(42)
