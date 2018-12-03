--TEST--
Bug #77231 (Segfault when using convert.quoted-printable-encode filter)
--FILE--
<?php
var_dump(file(urldecode('php://filter/convert.quoted-printable-encode/resource=data://,%bfAAAAAAAAFAAAAAAAAAAAAAA%ff%ff%ff%ff%ff%ff%ff%ffAAAAAAAAAAAAAAAAAAAAAAAA')));
?>
--EXPECT--
array(1) {
  [0]=>
  string(74) "=BFAAAAAAAAFAAAAAAAAAAAAAA=FF=FF=FF=FF=FF=FF=FF=FFAAAAAAAAAAAAAAAAAAAAAAAA"
}