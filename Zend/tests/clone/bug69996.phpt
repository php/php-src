--TEST--
Bug #69996 (Changing the property of a cloned object affects the original)
--FILE--
<?php

function method($cache) {
      $prepared = clone $cache;
      var_dump($prepared->data);
      $prepared->data = "bad";
      return $prepared;
}

$cache = new stdClass();
$cache->data = "good";

for ($i = 0; $i < 5; ++$i) {
       method($cache);
}
?>
--EXPECT--
string(4) "good"
string(4) "good"
string(4) "good"
string(4) "good"
string(4) "good"
