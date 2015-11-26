--TEST--
Bug #53319 (Strip_tags() may strip '<br />' incorrectly)
--FILE--
<?php

$str = '<br /><br  />USD<input type="text"/><br/>CDN<br><input type="text" />';
var_dump(strip_tags($str, '<input>'));
var_dump(strip_tags($str, '<br><input>') === $str);
var_dump(strip_tags($str));
var_dump(strip_tags('<a/b>', '<a>'));

?>
--EXPECTF--
string(47) "USD<input type="text"/>CDN<input type="text" />"
bool(true)
string(6) "USDCDN"
string(0) ""
