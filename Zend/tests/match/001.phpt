--TEST--
Basic match expression functionality test
--FILE--
<?php

function wordify($x) {
    return match ($x) {
        0 => 'Zero',
        1 => 'One',
        2 => 'Two',
        3 => 'Three',
        4 => 'Four',
        5 => 'Five',
        6 => 'Six',
        7 => 'Seven',
        8 => 'Eight',
        9 => 'Nine',
    };
}

for ($i = 0; $i <= 9; $i++) {
    print wordify($i) . "\n";
}

?>
--EXPECT--
Zero
One
Two
Three
Four
Five
Six
Seven
Eight
Nine
