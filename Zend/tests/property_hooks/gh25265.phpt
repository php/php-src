--TEST--
GH-25265 (Hooked property in a subclass of an internal class with virtual properties)
--CREDITS--
YuanchengJiang
--EXTENSIONS--
dom
--FILE--
<?php

class Test extends DOMDocument {
    public int $prop { set => $value; }
}

$doc = new Test;
$doc->prop = 42;
var_dump(get_object_vars($doc));
foreach ($doc as $key => $value) {
    var_dump($key, $value);
}

?>
--EXPECT--
array(1) {
  ["prop"]=>
  int(42)
}
string(4) "prop"
int(42)
