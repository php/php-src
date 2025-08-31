--TEST--
Use-after-free of MBSTRG(all_encodings_list) on shutdown
--EXTENSIONS--
mbstring
--FILE--
<?php
mb_list_encodings();
?>
--EXPECT--
