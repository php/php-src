--TEST--
GH-10192 (mb_detect_encoding() results for UTF-7 differ between PHP 8.0 and 8.1)
--EXTENSIONS--
mbstring
--FILE--
<?php

$testcases = [
    'non-base64 character after &' => 'A & B',
    'non-base64 character after -' => 'A - B',
    'base64 character before &' => 'A 1& B',
    'base64 character before -' => 'A 1- B',
    'base64 character after &' => 'A &1 B',
    'base64 character after -' => 'A -1 B',
    'base64 character before and after &' => 'A 1&1 B',
    'base64 character before and after -' => 'A 1-1 B',
    'string ends with &' => 'A &',
    'string ends with -' => 'A -',
    '& and -' => 'A &- B',
    '- and &' => 'A -& B',
    'valid direct encoding character ~' => 'A ~ B',
    'invalid direct encoding character ESC' => "A \x1b B",
    'valid direct encoding character ~ after &' => 'A &~ B',
    'invalid direct encoding character ESC after &' => "A &\x1b B",
    'valid base64 character between & and -' => 'A &ZeVnLIqe- B', // 日本語 in UTF-16BE
    'invalid base64 character between & and -' => 'A &ZeVnLIq- B', // 日本語 in UTF-16BE without the last character
    'valid base64 character between & and non-base64 character' => 'A &ZeVnLIqe B',
    'invalid base64 character between & and non-base64 character' => 'A &ZeVnLIq B',
    'valid base64 character between & and base64 character' => 'A &ZeVnLIqe1 B',
    'invalid base64 character between & and base64 character' => 'A &ZeVnLIq1 B',
    'valid base64 character between & and end of string' => 'A &ZeVnLIqe',
    'invalid base64 character between & and end of string' => 'A &ZeVnLIq',
    'valid base64 character using surrogate pair between & and -' => 'A &2GfePQ- B', // 𩸽 in UTF-16BE
    'first 16 bits of base64 character using surrogate pair between & and -' => 'A &2Gc- B', // first 16 bits of 𩸽 in UTF-16BE
    'valid base64 character using surrogate pair between & and non-base64 character' => 'A &2GfePQ B',
    'first 16 bits of base64 character using surrogate pair between & and non-base64 character' => 'A &2Gc B',
    'valid base64 character using surrogate pair between & and base64 character' => 'A &2GfePQ1 B',
    'first 16 bits of base64 character using surrogate pair between & and base64 character' => 'A &2Gc1 B',
    'valid base64 character using surrogate pair between & and end of string' => 'A &2GfePQ',
    'first 16 bits of base64 character using surrogate pair between & and end of string' => 'A &2Gc',
    'invalid base64 character using surrogate pair in reverse order between & and -' => 'A &3j3YZw- B', // 𩸽 in reverse order in UTF-16BE
    'last 16 bits of base64 character using surrogate pair in reverse order between & and -' => 'A &3j0- B', // last 16 bits of 𩸽 in UTF-16BE
    'invalid base64 character using surrogate pair in reverse order between & and non-base64 character' => 'A &3j3YZw B',
    'last 16 bits of base64 character using surrogate pair in reverse order between & and non-base64 character' => 'A &3j0 B',
    'invalid base64 character using surrogate pair in reverse order between & and base64 character' => 'A &3j3YZw1 B',
    'last 16 bits of base64 character using surrogate pair in reverse order between & and base64 character' => 'A &3j01 B',
    'invalid base64 character using surrogate pair in reverse order between & and end of string' => 'A &3j3YZw',
    'last 16 bits of base64 character using surrogate pair in reverse order between & and end of string' => 'A &3j0'
];

foreach ($testcases as $title => $case) {
    echo $title . PHP_EOL;
    var_dump(mb_detect_encoding($case, 'UTF-8, UTF7-IMAP', true));
    var_dump(mb_detect_encoding($case, 'UTF-8, UTF7-IMAP', false));
    var_dump(mb_detect_encoding($case, 'UTF7-IMAP', true));
    var_dump(mb_detect_encoding($case, 'UTF7-IMAP', false));
    var_dump(mb_check_encoding($case, 'UTF7-IMAP'));
    var_dump(addcslashes(mb_convert_encoding($case, 'UTF-8', 'UTF7-IMAP'), "\0..\37\177"));
    var_dump(mb_get_info('illegal_chars'));
    echo PHP_EOL;
}

?>
--EXPECT--
non-base64 character after &
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(4) "A ?B"
int(1)

non-base64 character after -
string(5) "UTF-8"
string(5) "UTF-8"
string(9) "UTF7-IMAP"
string(9) "UTF7-IMAP"
bool(true)
string(5) "A - B"
int(1)

base64 character before &
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(5) "A 1?B"
int(2)

base64 character before -
string(5) "UTF-8"
string(5) "UTF-8"
string(9) "UTF7-IMAP"
string(9) "UTF7-IMAP"
bool(true)
string(6) "A 1- B"
int(2)

base64 character after &
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(4) "A ?B"
int(3)

base64 character after -
string(5) "UTF-8"
string(5) "UTF-8"
string(9) "UTF7-IMAP"
string(9) "UTF7-IMAP"
bool(true)
string(6) "A -1 B"
int(3)

base64 character before and after &
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(5) "A 1?B"
int(4)

base64 character before and after -
string(5) "UTF-8"
string(5) "UTF-8"
string(9) "UTF7-IMAP"
string(9) "UTF7-IMAP"
bool(true)
string(7) "A 1-1 B"
int(4)

string ends with &
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(3) "A ?"
int(5)

string ends with -
string(5) "UTF-8"
string(5) "UTF-8"
string(9) "UTF7-IMAP"
string(9) "UTF7-IMAP"
bool(true)
string(3) "A -"
int(5)

& and -
string(9) "UTF7-IMAP"
string(9) "UTF7-IMAP"
string(9) "UTF7-IMAP"
string(9) "UTF7-IMAP"
bool(true)
string(5) "A & B"
int(5)

- and &
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(5) "A -?B"
int(6)

valid direct encoding character ~
string(5) "UTF-8"
string(5) "UTF-8"
string(9) "UTF7-IMAP"
string(9) "UTF7-IMAP"
bool(true)
string(5) "A ~ B"
int(6)

invalid direct encoding character ESC
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(5) "A ? B"
int(7)

valid direct encoding character ~ after &
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(5) "A ? B"
int(8)

invalid direct encoding character ESC after &
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(5) "A ? B"
int(9)

valid base64 character between & and -
string(9) "UTF7-IMAP"
string(9) "UTF7-IMAP"
string(9) "UTF7-IMAP"
string(9) "UTF7-IMAP"
bool(true)
string(13) "A 日本語 B"
int(9)

invalid base64 character between & and -
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(11) "A 日本? B"
int(10)

valid base64 character between & and non-base64 character
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(13) "A 日本語?B"
int(11)

invalid base64 character between & and non-base64 character
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(10) "A 日本?B"
int(12)

valid base64 character between & and base64 character
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(13) "A 日本語?B"
int(13)

invalid base64 character between & and base64 character
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(13) "A 日本誵?B"
int(14)

valid base64 character between & and end of string
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(12) "A 日本語?"
int(15)

invalid base64 character between & and end of string
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(9) "A 日本?"
int(16)

valid base64 character using surrogate pair between & and -
string(5) "UTF-8"
string(5) "UTF-8"
string(9) "UTF7-IMAP"
string(9) "UTF7-IMAP"
bool(true)
string(8) "A 𩸽 B"
int(16)

first 16 bits of base64 character using surrogate pair between & and -
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(5) "A ? B"
int(17)

valid base64 character using surrogate pair between & and non-base64 character
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(8) "A 𩸽?B"
int(18)

first 16 bits of base64 character using surrogate pair between & and non-base64 character
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(4) "A ?B"
int(19)

valid base64 character using surrogate pair between & and base64 character
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(8) "A 𩸽?B"
int(20)

first 16 bits of base64 character using surrogate pair between & and base64 character
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(4) "A ?B"
int(21)

valid base64 character using surrogate pair between & and end of string
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(7) "A 𩸽?"
int(22)

first 16 bits of base64 character using surrogate pair between & and end of string
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(4) "A ??"
int(24)

invalid base64 character using surrogate pair in reverse order between & and -
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(6) "A ?? B"
int(26)

last 16 bits of base64 character using surrogate pair in reverse order between & and -
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(5) "A ? B"
int(27)

invalid base64 character using surrogate pair in reverse order between & and non-base64 character
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(5) "A ??B"
int(29)

last 16 bits of base64 character using surrogate pair in reverse order between & and non-base64 character
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(5) "A ??B"
int(31)

invalid base64 character using surrogate pair in reverse order between & and base64 character
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(5) "A ??B"
int(33)

last 16 bits of base64 character using surrogate pair in reverse order between & and base64 character
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(5) "A ??B"
int(35)

invalid base64 character using surrogate pair in reverse order between & and end of string
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(5) "A ???"
int(38)

last 16 bits of base64 character using surrogate pair in reverse order between & and end of string
string(5) "UTF-8"
string(5) "UTF-8"
bool(false)
string(9) "UTF7-IMAP"
bool(false)
string(4) "A ??"
int(40)
