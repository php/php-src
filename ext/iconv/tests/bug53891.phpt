--TEST--
Bug #53891 (iconv_mime_encode() fails to Q-encode UTF-8 string)
--SKIPIF--
<?php
if (!extension_loaded('iconv')) die('skip iconv extension not available');
?>
--FILE--
<?php
$preferences = array(
    'scheme' => 'Q',
    'input-charset'  => 'utf-8',
    'output-charset' => 'utf-8',
    'line-length' => 74,
    'line-break-chars' => "\r\n",
);
var_dump(iconv_mime_encode('subject', "d obeybiubrsfqllpdtpge…", $preferences));
?>
===DONE===
--EXPECT--
string(54) "subject: =?utf-8?Q?d=20obeybiubrsfqllpdtpge=E2=80=A6?="
===DONE===
