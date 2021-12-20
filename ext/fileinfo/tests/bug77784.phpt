--TEST--
Bug #77784 mime_content_type result gets doubled for xlsx
--FILE--
<?php
var_dump(mime_content_type('./ext/fileinfo/tests/bug77784.xlsx'));
?>
--EXPECT--
string(65) "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"
