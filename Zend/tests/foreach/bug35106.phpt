--TEST--
Bug #35106 (nested foreach fails when array variable has a reference)
--FILE--
<?php
$a=array("1","2");
$b=&$a;
foreach($a as $i){
    echo $i;
    foreach($a as $p);
}
echo "\n";
?>
--EXPECT--
12
