--TEST--
Bug GH-9498 (unicode string corruption during SELECT)
--EXTENSIONS--
pdo_odbc
--SKIPIF--
<?php
require 'ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->setAttribute(\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);

$select_test = "SELECT
    CAST(0x72006f017e016f007600fd00 AS nvarchar(6)) AS pink_in_czech_binary_encoded_utf16le
  , CAST(0x6b26 AS nvarchar(1)) AS beamed_eighth_notes_binary_encoded_utf16le
  , NCHAR(0x266b) AS beamed_eighth_notes_nchar
  , N'♫' AS \"beamed_eighth_notes_u+266b_constant\"
  , N'růžový' AS pink_in_czech_unicode_string_constant
  , NCHAR(0xfb01) AS ligature_fi_nchar
  , N'𝄋' AS \"segno_u+1d10b_constant\"
;";
var_dump($db->query($select_test, \PDO::FETCH_ASSOC)->fetchAll());
?>
--EXPECT--
array(1) {
  [0]=>
  array(7) {
    ["pink_in_czech_binary_encoded_utf16le"]=>
    string(9) "růžový"
    ["beamed_eighth_notes_binary_encoded_utf16le"]=>
    string(3) "♫"
    ["beamed_eighth_notes_nchar"]=>
    string(3) "♫"
    ["beamed_eighth_notes_u+266b_constant"]=>
    string(3) "♫"
    ["pink_in_czech_unicode_string_constant"]=>
    string(9) "růžový"
    ["ligature_fi_nchar"]=>
    string(3) "ﬁ"
    ["segno_u+1d10b_constant"]=>
    string(4) "𝄋"
  }
}
