--TEST--
Bug #24396 (global $$variable broken)
--FILE--
<?php

$arr = array('a' => 1, 'b' => 2, 'c' => 3);

foreach($arr as $k=>$v)  {
    global $$k; // comment this out and it works in PHP 5 too..
               
    echo "($k => $v)\n";
                       
    $$k = $v;          
}
?>
--EXPECT--
(a => 1)
(b => 2)
(c => 3)
