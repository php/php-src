--TEST--
Try catch finally (multi catch blocks with return)
--FILE--
<?php

class AE extends Exception {};
class BE extends Exception {};

function foo () {
    try {
        try {
            try {
                throw new Exception("try");
            } catch (AE $e) {
                die("error");
            } finally {
               echo "1";
            }
        } finally {
           echo "2";
        }
    } catch (BE $e) {
      die("error");
    } catch (Exception $e) {
        echo "3";
    } finally {
        echo "4";
        return 4;
    }
   return 5;
}

var_dump(foo());
?>
--EXPECT--
1234int(4)
