--TEST--
mb_list_encodings
--EXTENSIONS--
mbstring
--FILE--
<?php
var_dump(in_array("7bit", mb_list_encodings()));
var_dump(in_array("8bit", mb_list_encodings()));
var_dump(in_array("ASCII", mb_list_encodings()));
var_dump(in_array("non-existent", mb_list_encodings()));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
