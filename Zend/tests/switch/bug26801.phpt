--TEST--
Bug #26801 (switch ($a{0}) crash)
--FILE--
<?php

$a = '11';
$b = $a[0];
switch ($b) {
    case '-':
        break;
}

$a = '22';
switch ($a[0]) {
    case '-':
        break;
}

?>
===DONE===
--EXPECT--
===DONE===
