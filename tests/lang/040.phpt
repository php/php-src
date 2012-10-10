--TEST--
foreach into array
--FILE--
<?php
$a = array(0,1);
$b[0]=2;
foreach($a as $b[0]) {
  echo $b[0]."\n";
}
?>
===DONE===
--EXPECT--
0
1
===DONE===
