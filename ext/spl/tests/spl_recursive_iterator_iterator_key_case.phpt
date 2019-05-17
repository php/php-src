--TEST--
SPL: Test on RecursiveIteratorIterator key function checking switch statements
--CREDITS--
Rohan Abraham (rohanabrahams@gmail.com)
TestFest London May 2009
--FILE--
<?php
  $ar = array("one"=>1, "two"=>2, "three"=>array("four"=>4, "five"=>5, "six"=>array("seven"=>7)), "eight"=>8, -100 => 10, NULL => "null");
  $it = new RecursiveArrayIterator($ar);
  $it = new RecursiveIteratorIterator($it);
  foreach($it as $k=>$v)
  {
    echo "$k=>$v\n";
    var_dump($k);
  }
?>
--EXPECT--
one=>1
string(3) "one"
two=>2
string(3) "two"
four=>4
string(4) "four"
five=>5
string(4) "five"
seven=>7
string(5) "seven"
eight=>8
string(5) "eight"
-100=>10
int(-100)
=>null
string(0) ""
