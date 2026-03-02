--TEST--
Bug #41034 (json_encode() ignores null byte started keys in arrays)
--FILE--
<?php
echo json_encode(array(0, "\0ab"=>1, "\0null-prefixed value"));
echo "\nDone\n";
?>
--EXPECT--
{"0":0,"\u0000ab":1,"1":"\u0000null-prefixed value"}
Done
