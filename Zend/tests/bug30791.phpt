--TEST--
Bug #30791 magic methods (__sleep/__wakeup/__toString) call __call if object is overloaded 
--FILE--
<?php
class a {
   public $a = 4;
   function __call($a,$b) {
       return "unknown method";
   }
}
$b = new a;
echo $b,"\n";
$c = unserialize(serialize($b));
echo $c,"\n";
var_dump($c);
?>
--EXPECT--
Object id #1
Object id #2
object(a)#2 (1) {
  ["a"]=>
  int(4)
}
