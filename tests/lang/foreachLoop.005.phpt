--TEST--
Foreach loop tests - modifying the array during the loop: special case. Behaviour is good since php 5.2.2.
--FILE--
<?php
$a = array("original.0","original.1","original.2");
foreach ($a as $k=>&$v){
  $a[$k] = "changed.$k";
  echo "After changing \$a directly, \$v@$k is: $v\n";
}
//--- Expected output:
//After changing $a directly, $v@0 is: changed.0
//After changing $a directly, $v@1 is: changed.1
//After changing $a directly, $v@2 is: changed.2
//--- Actual output from php.net before 5.2.2:
//After changing $a directly, $v@0 is: changed.0
//After changing $a directly, $v@1 is: original.1
//After changing $a directly, $v@2 is: original.2

?>
--EXPECT--
After changing $a directly, $v@0 is: changed.0
After changing $a directly, $v@1 is: changed.1
After changing $a directly, $v@2 is: changed.2
