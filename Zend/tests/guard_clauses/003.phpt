--TEST--
break if
--FILE--
<?php

$x = 0;

for ($i = 0; $i < 10; $i ++) {
    break if ($i >= 5);
    
    $x++;
}

$y = 0;

for ($j = 0; $j < 10; $j++) {
    for ($k = 0; $k < 10; $k++) {
        break if ($j >= 5): 2;
        
        $y++;
    }
}

$output = 'original';

switch (1) {
    case 1:
        switch (2) {
            case 2:
                break if (true): 2;
        }

        // never executed
        $output = 'changed';
        break;
}

var_dump($x);
var_dump($i);
var_dump($y);
var_dump($j);
var_dump($k);
var_dump($output);

?>
--EXPECT--
int(5)
int(5)
int(50)
int(5)
int(0)
string(8) "original"