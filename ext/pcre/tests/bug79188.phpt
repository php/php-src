--TEST--
Bug #79188: Memory corruption in preg_replace/preg_replace_callback and unicode
--FILE--
<?php

var_dump(preg_replace("//u", "", "a" . str_repeat("\u{1f612}", 10)));
var_dump(preg_replace_callback(
    "//u", function() { return ""; }, "a" . str_repeat("\u{1f612}", 10)));

?>
--EXPECT--
string(41) "a😒😒😒😒😒😒😒😒😒😒"
string(41) "a😒😒😒😒😒😒😒😒😒😒"
