--TEST--
Bug #78271: Invalid result of if-else
--FILE--
<?php
function test($a, $b){
    if ($a==10) {
        $w="x";
    } else {
        $w="y";
    }

    if ($b) {
        $d1="none";
        $d2="block";
    } else {
        $d1="block";
        $d2="none";
    }

    echo $d2.$b."\n";

}

test(1, 1);
?>
--EXPECT--
block1
