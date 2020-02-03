--TEST--
Nested If/ElseIf/Else Test
--FILE--
<?php
$a=1;
$b=2;

if($a==0) {
    echo "bad";
} elseif($a==3) {
    echo "bad";
} else {
    if($b==1) {
        echo "bad";
    } elseif($b==2) {
        echo "good";
    } else {
        echo "bad";
    }
}
?>
--EXPECT--
good
