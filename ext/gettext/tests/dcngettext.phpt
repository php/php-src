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
var_dump(dcngettext("test","test","test",0,0));
var_dump(dcngettext("test","test","test",-1,-1));
var_dump(dcngettext("","","",1,1));
var_dump(dcngettext("","","",0,0));

echo "Done\n";
?>
--EXPECT--
string(1) "1"
string(4) "test"
string(4) "test"
string(4) "test"
string(0) ""
string(0) ""
Done
