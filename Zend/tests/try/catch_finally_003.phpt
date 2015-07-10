--TEST--
Try catch finally (with multi-returns)
--FILE--
<?php
function dummy($msg) {
   var_dump($msg);
}

function foo ($a) {
   try {
       dummy("try");
       return $a;
   } catch (Exception $e) {
       throw $e;
   } finally {
       dummy("finally");
       return "finally";
   }
   return "end";
}

function &bar($a) {
   try {
     echo "try\n";
     throw new Exception("ex");
   } catch (Exception $e) {
   } finally {
     return $a;
   }
   return ($c = "end");
}
var_dump(foo("para"));
var_dump(bar("para"));
?>
--EXPECTF--
string(3) "try"
string(7) "finally"
string(7) "finally"
try
string(4) "para"
