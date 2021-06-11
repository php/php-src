--TEST--
htmlentities() test 8 (mbstring / EUC-JP)
--INI--
output_handler=
internal_encoding=EUC-JP
--EXTENSIONS--
mbstring
--FILE--
<?php
    mb_internal_encoding('EUC-JP');
    print mb_internal_encoding()."\n";
    var_dump(htmlentities("\xa1\xa2\xa1\xa3\xa1\xa4", ENT_QUOTES, ''));
?>
--EXPECTF--
EUC-JP

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
string(6) "¡¢¡£¡¤"
