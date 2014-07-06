--TEST--
Try catch finally
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
                echo "0";
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
    }
   return 1;
}

var_dump(foo());
?>
--EXPECTF--
1234int(1)
