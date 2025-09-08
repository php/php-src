--TEST--
htmlentities() test 7 (mbstring / ISO-8859-1)
--INI--
output_handler=
internal_encoding=ISO-8859-1
--EXTENSIONS--
mbstring
--FILE--
<?php
    print mb_internal_encoding()."\n";
    var_dump(htmlentities("\xe4\xf6\xfc", ENT_QUOTES, ''));
?>
--EXPECT--
ISO-8859-1
string(18) "&auml;&ouml;&uuml;"
