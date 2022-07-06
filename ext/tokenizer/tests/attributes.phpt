--TEST--
Attributes are exposed as tokens.
--FILE--
<?php

$tokens = token_get_all('<?php #[A1(1, 2)] class C1 { }');

$attr = $tokens[1];
var_dump(token_name(T_ATTRIBUTE));
var_dump($attr[0] === T_ATTRIBUTE);
var_dump($attr[1]);

$class = $tokens[2];
var_dump($class[1]);
?>
--EXPECT--
string(11) "T_ATTRIBUTE"
bool(true)
string(2) "#["
string(2) "A1"
