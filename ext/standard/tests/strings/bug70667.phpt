--TEST--
Bug #70667 (strtr() causes invalid writes and a crashes)
--FILE--
<?php
$a = array("{{language_id}}"=>"255", "{{partner_name}}"=>"test1");
var_dump(strtr("Sign in to test1", $a));
?>
--EXPECT--
string(16) "Sign in to test1"
