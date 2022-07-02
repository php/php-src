--TEST--
Testing user-defined function falling out of an If into another
--FILE--
<?php
$a = 1;
function Test ($a) {
    if ($a<3) {
        return(3);
    }
}

if ($a < Test($a)) {
    echo "$a\n";
    $a++;
}
?>
--EXPECT--
1
