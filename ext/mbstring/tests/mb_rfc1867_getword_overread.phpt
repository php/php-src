--TEST--
Over-read in php_mb_rfc1867_getword() on a multibyte field name (mbstring.encoding_translation=On)
--EXTENSIONS--
mbstring
--POST_RAW--
Content-Type: multipart/form-data; boundary=blah

--blah
Content-Disposition: form-data; name=a×

x
--blah--
--INI--
mbstring.encoding_translation=On
--FILE--
<?php
var_dump(count($_POST));
echo "Done\n";
?>
--EXPECT--
int(1)
Done
