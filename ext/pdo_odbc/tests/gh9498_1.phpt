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

$tests = [
    [
        "name" => "beamed_eighth_notes_binary_encoded_utf16le",
        "expr" => "CAST(0x6b26 AS nvarchar(1))",
        "expected_answer" => "♫",
    ],
    [
        "name" => "beamed_eighth_notes_nchar",
        "expr" => "NCHAR(0x266b)",
        "expected_answer" => "♫",
    ],
    [
        "name" => "beamed_eighth_notes_u+266b_constant",
        "expr" => "N'♫'",
        "expected_answer" => "♫",
    ],
    [
        "name" => "ligature_fi_nchar",
        "expr" => "NCHAR(0xfb01)",
        "expected_answer" => "ﬁ",
    ],
    [
        "name" => "php_ascii_string_constant",
        "expr" => "'php'",
        "expected_answer" => "php",
    ],
    [
        "name" => "php_unicode_string_constant",
        "expr" => "N'php'",
        "expected_answer" => "php",
    ],
    [
        "name" => "pink_in_czech_binary_encoded_utf16le",
        "expr" => "CAST(0x72006f017e016f007600fd00 AS nvarchar(6))",
        "expected_answer" => "růžový",
    ],
    [
        "name" => "pink_in_czech_unicode_string_constant",
        "expr" => "N'růžový'",
        "expected_answer" => "růžový",
    ],
    [
        "name" => "segno_u+1d10b_constant",
        "expr" => "N'𝄋'",
        "expected_answer" => "𝄋",
    ],
];

foreach ($tests as $t) {
    print $t["name"] . ": ";
    $sql = "SELECT " . $t["expr"] . ' AS "' . $t["name"] . '";';
    $results = $db->query($sql, \PDO::FETCH_NUM)->fetch();
    if ( $results[0] == $t["expected_answer"] ) {
        print "PASS";
    } else {
        print "FAIL!";
        print "\tresult: '" . $results[0] . "'";
        print "\texpected: '" . $t["expected_answer"] . "'";
    }
    print "\n";
}
?>
--EXPECT--
beamed_eighth_notes_binary_encoded_utf16le: PASS
beamed_eighth_notes_nchar: PASS
beamed_eighth_notes_u+266b_constant: PASS
ligature_fi_nchar: PASS
php_ascii_string_constant: PASS
php_unicode_string_constant: PASS
pink_in_czech_binary_encoded_utf16le: PASS
pink_in_czech_unicode_string_constant: PASS
segno_u+1d10b_constant: PASS
