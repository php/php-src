--TEST--
Bug #78236 (convert error on receiving variables when duplicate [)
--POST--
id[name=1&id[[name=a&id[na me.=3
--FILE--
<?php
var_dump($_POST);
?>
--EXPECT--
array(3) {
  ["id_name"]=>
  string(1) "1"
  ["id__name"]=>
  string(1) "a"
  ["id_na_me_"]=>
  string(1) "3"
}
