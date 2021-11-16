--TEST--
Bug #66828 (iconv_mime_encode Q-encoding longer than it should be)
--EXTENSIONS--
iconv
--FILE--
<?php
$preferences = array(
    "input-charset" => "ISO-8859-1",
    "output-charset" => "UTF-8",
    "line-length" => 76,
    "line-break-chars" => "\n",
    "scheme" => "Q"
);
var_dump(iconv_mime_encode("Subject", "Test Test Test Test Test Test Test Test", $preferences));
?>
--EXPECT--
string(74) "Subject: =?UTF-8?Q?Test=20Test=20Test=20Test=20Test=20Test=20Test=20Test?="
