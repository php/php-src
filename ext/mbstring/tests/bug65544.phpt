--TEST--
Bug #65544: mb title case conversion-first word in quotation isn't capitalized
--EXTENSIONS--
mbstring
--FILE--
<?php
var_dump(mb_convert_case("\"or else it doesn't, you know. the name of the song is called 'haddocks' eyes.'\"", MB_CASE_TITLE));
?>
--EXPECT--
string(80) ""Or Else It Doesn't, You Know. The Name Of The Song Is Called 'Haddocks' Eyes.'""
