--TEST--
Bug #44703 (htmlspecialchars() does not detect bad character set argument)
--FILE--
<?php

var_dump(htmlspecialchars("<a href='test'>Test</a>", ENT_COMPAT, 1));
var_dump(htmlspecialchars("<a href='test'>Test</a>", ENT_COMPAT, 12));
var_dump(htmlspecialchars("<a href='test'>Test</a>", ENT_COMPAT, 125));
var_dump(htmlspecialchars("<a href='test'>Test</a>", ENT_COMPAT, 1252));
var_dump(htmlspecialchars("<a href='test'>Test</a>", ENT_COMPAT, 12526));

var_dump(htmlspecialchars("<>", ENT_COMPAT, 866));
var_dump(htmlspecialchars("<>", ENT_COMPAT, 8666));

var_dump(htmlspecialchars("<>", ENT_COMPAT, NULL));


var_dump(htmlspecialchars("<>", ENT_COMPAT, 'SJIS'));
var_dump(htmlspecialchars("<>", ENT_COMPAT, 'SjiS'));

var_dump(htmlspecialchars("<>", ENT_COMPAT, str_repeat('a', 100)));

?>
--EXPECTF--
Warning: htmlspecialchars(): charset `1' not supported, assuming iso-8859-1 in %s on line %d
string(35) "&lt;a href='test'&gt;Test&lt;/a&gt;"

Warning: htmlspecialchars(): charset `12' not supported, assuming iso-8859-1 in %s on line %d
string(35) "&lt;a href='test'&gt;Test&lt;/a&gt;"

Warning: htmlspecialchars(): charset `125' not supported, assuming iso-8859-1 in %s on line %d
string(35) "&lt;a href='test'&gt;Test&lt;/a&gt;"
string(35) "&lt;a href='test'&gt;Test&lt;/a&gt;"

Warning: htmlspecialchars(): charset `12526' not supported, assuming iso-8859-1 in %s on line %d
string(35) "&lt;a href='test'&gt;Test&lt;/a&gt;"
string(8) "&lt;&gt;"

Warning: htmlspecialchars(): charset `8666' not supported, assuming iso-8859-1 in %s on line %d
string(8) "&lt;&gt;"
string(8) "&lt;&gt;"
string(8) "&lt;&gt;"
string(8) "&lt;&gt;"

Warning: htmlspecialchars(): charset `aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa' not supported, assuming iso-8859-1 in %s on line %d
string(8) "&lt;&gt;"


