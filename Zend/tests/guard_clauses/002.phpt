--TEST--
continue if
--FILE--
<?php

$x = 0;

for ($i = 0; $i < 10; $i ++) {
    continue if ($i >= 5);
    
    $x++;
}

$y = 0;

for ($j = 0; $j < 10; $j++) {
    for ($k = 0; $k < 10; $k++) {
        continue if ($j >= 5): 2;
        
        $y++;
    }
}

var_dump($x);
var_dump($i);
var_dump($y);
var_dump($j);
var_dump($k);


?>
--EXPECT--
int(5)
int(10)
int(50)
int(10)
int(0)
