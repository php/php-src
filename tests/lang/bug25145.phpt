--TEST--
Bug #25145 (SEGV on recpt of form input with name like "123[]")
--SKIPIF--
<?php if (php_sapi_name() == 'cli') echo 'skip'; ?>
--GET--
123[]=SEGV
--FILE--
<?php

var_dump($_REQUEST);
echo "Done\n";

?>
--EXPECT--
array(1) {
  [123]=>
  array(1) {
    [0]=>
    string(4) "SEGV"
  }
}
Done
