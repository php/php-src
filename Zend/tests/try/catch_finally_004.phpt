--TEST--
Try catch finally (nesting try-catch-finally)
--FILE--
<?php

function throw_exception($msg) {
    throw new Exception($msg);
}

function foo (&$ex) {
   try {
      echo "1";
      try {
        echo "2";
        throw_exception("try");
      } catch (Exception $e) {
        echo "3";
        throw_exception("catch");
      } finally {
        echo "4";
        throw_exception("finally");
      } 
   } catch (Exception $e) {
      $ex = $e;
      echo "3";
   } finally {
      echo "2";
   }
   return 1;
}

var_dump(foo($ex));

do {
  var_dump($ex->getMessage());
} while ($ex = $ex->getPrevious());
?>
--EXPECT--
123432int(1)
string(7) "finally"
string(5) "catch"
