--TEST--
Simple Switch Test
--FILE--
<?php
$a=1;
switch($a) {
    case 0:
        echo "bad";
        break;
    case 1:
        echo "good";
        break;
    default:
        echo "bad";
        break;
}
?>
--EXPECT--
good
