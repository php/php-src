--TEST--
GH-19397 (mb_list_encodings() can cause crashes on shutdown)
--EXTENSIONS--
mbstring
--FILE--
<?php
$doNotDeleteThisVariableAssignment = mb_list_encodings();
var_dump(count($doNotDeleteThisVariableAssignment) > 0);
?>
--EXPECT--
bool(true)
