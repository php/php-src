--TEST--
Bug #64346: Function name resolution and eval
--FILE--
<?php

namespace NS;

function getLen($s) {
    return strlen($s);
}

var_dump(getLen("foo"));
eval("namespace NS_unrelated; function strlen() { return 42; }");
var_dump(getLen("foo"));
eval("namespace NS; function strlen() { return 42; }");
var_dump(getLen("foo"));

?>
--EXPECT--
int(3)
int(3)
int(42)
