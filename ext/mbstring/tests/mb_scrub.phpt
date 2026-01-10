--TEST--
mb_scrub()
--EXTENSIONS--
mbstring
--FILE--
<?php
var_dump(
    "?" === mb_scrub("\x80"),
    "?" === mb_scrub("\x80", 'UTF-8')
);

$utf8str = "abc 日本語 Οὐχὶ ταὐτὰ παρίσταταί μοι γιγνώσκειν ⡍⠔⠙⠖ ⡊ ⠙⠕⠝⠰⠞";
// Check $utf8str so it is marked as 'valid UTF-8'
// This will enable optimized implementation of mb_scrub
if (!mb_check_encoding($utf8str, 'UTF-8'))
    die("Test string should be valid UTF-8");
var_dump(mb_scrub($utf8str));
?>
--EXPECT--
bool(true)
bool(true)
string(122) "abc 日本語 Οὐχὶ ταὐτὰ παρίσταταί μοι γιγνώσκειν ⡍⠔⠙⠖ ⡊ ⠙⠕⠝⠰⠞"
