--TEST--
Segfault in mb_fast_convert() when mbstring.encoding_translation is enabled (GH-13123)
--EXTENSIONS--
mbstring
--POST_RAW--
Content-Type: multipart/form-data, boundary=Blah

--Blah
Content-Disposition: form-data; name="file"; filename="file.txt"
Content-Type: text/plain

foo
--Blah

--INI--
error_reporting=E_ALL&~E_DEPRECATED
mbstring.encoding_translation=On
mbstring.http_input=pass
--FILE--
<?php
print "Done!\n";
?>
--EXPECT--
Done!
