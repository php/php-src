--TEST--
mb_ereg_search() stuff
--SKIPIF--
<?php include('skipif.inc'); ?>
function_exists('mb_ereg_search') or die("SKIP");
--POST--
--GET--
--FILE--
<?php include('024.inc'); ?>
--EXPECT--
(EUC-JP) (10) ¡¦¢Ï
(EUC-JP) (5) abcde
(EUC-JP) (14) abdeabcf
(EUC-JP) (22) abc
(EUC-JP) (31) abcd
(EUC-JP) (5) ¢Ï¡¦
(EUC-JP) (10) ¡¦¢Ï
(EUC-JP) (5) abcde
(EUC-JP) (14) abdeabcf
(EUC-JP) (22) abc
(EUC-JP) (31) abcd
(Shift_JIS) (10) ¡¦¢Ï
(Shift_JIS) (5) abcde
(Shift_JIS) (14) abdeabcf
(Shift_JIS) (22) abc
(Shift_JIS) (31) abcd
(Shift_JIS) (5) ¢Ï¡¦
(Shift_JIS) (10) ¡¦¢Ï
(Shift_JIS) (5) abcde
(Shift_JIS) (14) abdeabcf
(Shift_JIS) (22) abc
(Shift_JIS) (31) abcd
(SJIS) (10) ¡¦¢Ï
(SJIS) (5) abcde
(SJIS) (14) abdeabcf
(SJIS) (22) abc
(SJIS) (31) abcd
(SJIS) (5) ¢Ï¡¦
(SJIS) (10) ¡¦¢Ï
(SJIS) (5) abcde
(SJIS) (14) abdeabcf
(SJIS) (22) abc
(SJIS) (31) abcd
(UTF-8) (14) ¡¦¢Ï
(UTF-8) (5) abcde
(UTF-8) (14) abdeabcf
(UTF-8) (22) abc
(UTF-8) (31) abcd
(UTF-8) (7) ¢Ï¡¦
(UTF-8) (14) ¡¦¢Ï
(UTF-8) (5) abcde
(UTF-8) (14) abdeabcf
(UTF-8) (22) abc
(UTF-8) (31) abcd
