--TEST--
foreach() with foreach($o->mthd()->arr)
--FILE--
<?php
class Test {
   public $a = array(1,2,3,4,5); // removed, crash too
   function c() {
      return new Test();
   }

}
$obj = new Test();
foreach ($obj->c()->a as $value) {
    print "$value\n";
}

?>
===DONE===
--EXPECT--
1
2
3
4
5
===DONE===
