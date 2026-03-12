--TEST--
Property existence test can cause a crash
--EXTENSIONS--
zip
--FILE--
<?php

$archive = new ZipArchive(__DIR__.'/property_existence.zip');
var_dump(array_column([$archive], 'lastId'));

?>
--CLEAN--
<?php
@unlink(__DIR__.'/property_existence.zip');
?>
--EXPECT--
array(1) {
  [0]=>
  int(-1)
}
