--TEST--
dcngettext() tests
--EXTENSIONS--
gettext
--SKIPIF--
<?php
if (!function_exists("dcngettext")) die("skip dcngettext() doesn't exist");
?>
--FILE--
<?php

var_dump(dcngettext(1,1,1,1,1));
var_dump(dcngettext("test","test","test",1,1));
var_dump(dcngettext("test","test","test",0,1));
var_dump(dcngettext("test","test","test",-1,-1));

try {
    dcngettext("","","",1,1);
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    dcngettext("","","",0,1);
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
string(1) "1"
string(4) "test"
string(4) "test"
string(4) "test"
dcngettext(): Argument #1 ($domain) cannot be empty
dcngettext(): Argument #1 ($domain) cannot be empty
Done
