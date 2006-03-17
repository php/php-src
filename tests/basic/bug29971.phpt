--TEST--
Bug #29971 (variables_order behaviour)
--INI--
variables_order=GPC
register_argc_argv=0
--FILE--
<?php
var_dump($_ENV,$_SERVER);
var_dump(ini_get("variables_order"));
?>
--EXPECT--
array(0) {
}
array(0) {
}
string(3) "GPC"
--UEXPECT--
array(0) {
}
array(0) {
}
unicode(3) "GPC"
