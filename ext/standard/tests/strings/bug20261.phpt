--FILE--
Bug #20261 (str_rot13() changes too much)
--FILE--
<?php
 $first = "boo";
 $second = $first;
 $rot = "";

 echo "1: ".$first."\n";
 echo "2: ".$second."\n";
 echo "3: ".$rot."\n";

 $rot = str_rot13($second);

 echo "1: ".$first."\n";
 echo "2: ".$second."\n";
 echo "3: ".$rot."\n";
?>
--EXPECT--
1: boo
2: boo
3: 
1: boo
2: boo
3: obb

