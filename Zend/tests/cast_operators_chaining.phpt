--TEST--
Cast operators chaining and precedence
--FILE--
<?php

var_dump((?string) (?int) "123");
var_dump((!float) (!int) "456");

var_dump((?int) null ?? 100);
var_dump(((?int) null) ?? 100);

var_dump((!int) "5" + (!int) "10");
var_dump((!int) "20" * 2);

?>
--EXPECT--
string(3) "123"
float(456)
int(100)
int(100)
int(15)
int(40)
