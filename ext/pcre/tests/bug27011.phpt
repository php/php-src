--TEST--
Bug #27011 (segfault in preg_match_all())
--FILE--
<?php

var_dump(preg_match_all('|(\w+)://([^\s"<]*[\w+#?/&=])|', "This is a text string", $matches, PREG_SET_ORDER));
var_dump($matches);

?>
--EXPECT--
int(0)
array(0) {
}
