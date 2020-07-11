--TEST--
Bug #77147 (Fixing 60494 ignored ICONV_MIME_DECODE_CONTINUE_ON_ERROR)
--SKIPIF--
<?php
if (!extension_loaded('iconv')) die('skip iconv extension not available');
?>
--FILE--
<?php
$string = <<<EOF
Feedback-ID: 014a93e3-1f5e-4df6-b347-6b59f0f746b8:b5891053-55d1-45bc-a0b5-47a7a9b59687:email:epslh1�
EOF;
$headers = iconv_mime_decode_headers($string, ICONV_MIME_DECODE_CONTINUE_ON_ERROR);
var_dump($headers);
?>
--EXPECT--
array(1) {
  ["Feedback-ID"]=>
  string(86) "014a93e3-1f5e-4df6-b347-6b59f0f746b8:b5891053-55d1-45bc-a0b5-47a7a9b59687:email:epslh1"
}
