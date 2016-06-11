--TEST--
Try catch finally (goto in try/catch block)
--CREDITS--
adoy
--FILE--
<?php
function foo($ret = FALSE) {
    try {
        try {
            do {
                goto label;
            } while(0);
            foreach (array() as $val) {
                continue;
            }
        } finally {
            var_dump("finally1");
            throw new Exception("exception");
        } 
    } catch (Exception $e) {
        goto local;
local: 
        var_dump("catched");
        if ($ret) return "return";
    } finally {
       var_dump("finally2");
    }

label:
   var_dump("label");
}

var_dump(foo());
var_dump(foo(true));

?>
--EXPECTF--
string(8) "finally1"
string(7) "catched"
string(8) "finally2"
string(5) "label"
NULL
string(8) "finally1"
string(7) "catched"
string(8) "finally2"
string(6) "return"
