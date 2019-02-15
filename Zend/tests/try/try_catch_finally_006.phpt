--TEST--
Try catch finally (goto in try/catch block)
--CREDITS--
adoy
--FILE--
<?php
function foo($ex = NULL) {
    try {
        try {
            goto label;
        } finally {
            var_dump("finally1");
            if ($ex) throw $ex;
        }
    } catch (Exception $e) {
       var_dump("catched");
       if ($ex) return "return1";
    } finally {
       var_dump("finally2");
    }

label:
   var_dump("label");
   return "return2";
}

var_dump(foo());
var_dump(foo(new Exception()));

?>
--EXPECT--
string(8) "finally1"
string(8) "finally2"
string(5) "label"
string(7) "return2"
string(8) "finally1"
string(7) "catched"
string(8) "finally2"
string(7) "return1"
