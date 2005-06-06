--TEST--
Bug #32799 (crash: calling the corresponding global var during the destruct)
--FILE--
<?php
class test{
  public $c=1;
  function __destruct (){
    $GLOBALS['p']->c++; // no warning
    print $GLOBALS['p']->c."\n"; // segfault
  	var_dump($GLOBALS['p']);
  }
}
$p=new test;
$p=null; //destroy the object by a new assignment (segfault)
?>
--EXPECT--
2
object(test)#1 (1) {
  ["c"]=>
  int(2)
}
