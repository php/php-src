--TEST--
Try finally (call sequence test)
--FILE--
<?php
function foo () {
   try {
      echo "1";
      try {
        echo "2";
        throw new Exception("ex");
      } finally {
        echo "3";
      }
   } finally {
      echo "4";
   }
}

foo();
?>
--EXPECTF--
1234
Exception: ex in %s on line %d
Stack trace:
#0 %stry_finally_003.php(%d): foo()
#1 {main}
