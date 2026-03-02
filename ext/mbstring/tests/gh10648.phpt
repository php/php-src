--TEST--
GH-10648 (mb_check_encoding() returns true for incorrect but interpretable ISO-2022-JP byte sequences)
--EXTENSIONS--
mbstring
--FILE--
<?php

$testcases = [
    'ISO-2022-JP bytes' => '1b244224221b2842', // 'あ' in ISO-2022-JP
    'ISO-2022-JP bytes without escape sequence' => '1b24422422', // 'ｱ' in JIS
    'JIS X 0201 7bit kana with escape sequence' => '1b2849311b2842', // 'ｱ' in JIS
    'JIS X 0201 7bit kana with SO/SI' => '0e310f', // 'ｱ' in JIS
    'JIS X 0201 8bit kana' => 'b1', // 'ｱ' in JIS
    'JIS X 0201 7bit kana with SO and ESC' => '0e311b2842', // 'ｱ' in JIS
    'JIS X 0201 7bit kana with ESC and SI' => '1b2849310f', // 'ｱ' in JIS
    'JIS X 0208 character' => '1b244242641b2842', // '鯛' in JIS and ISO-2022-JP, included in JIS X 0208
    'JIS X 0212 character' => '1b2428446a591b2842', // '鮋' in JIS, included in JIS X 0212
    'JIS X 0213 character' => '1b2428507d4c1b2842', // '𩸽' in ISO-2022-JP-2004, included in JIS X 0213
    'JIS C 6220-1969 ESC ( H' => '1b284a1b2848', // an escape sequence transitioning to ASCII
    'SO/SI when not in ASCII mode' => '1b284a0e0f1b2842', // an escape sequence transitioning to ASCII
];

foreach ($testcases as $title => $case) {
    echo $title . PHP_EOL;
    echo 'JIS:' . PHP_EOL;
    var_dump(mb_check_encoding(hex2bin($case), 'JIS'));
    echo mb_convert_encoding(hex2bin($case), 'UTF-8', 'JIS'). PHP_EOL;
    var_dump(mb_get_info('illegal_chars'));
    echo 'ISO-2022-JP:' . PHP_EOL;
    var_dump(mb_check_encoding(hex2bin($case), 'ISO-2022-JP'));
    echo mb_convert_encoding(hex2bin($case), 'UTF-8', 'ISO-2022-JP'). PHP_EOL;
    var_dump(mb_get_info('illegal_chars'));
    echo PHP_EOL;
}
?>
--EXPECT--
ISO-2022-JP bytes
JIS:
bool(true)
あ
int(0)
ISO-2022-JP:
bool(true)
あ
int(0)

ISO-2022-JP bytes without escape sequence
JIS:
bool(false)
あ
int(0)
ISO-2022-JP:
bool(false)
あ
int(0)

JIS X 0201 7bit kana with escape sequence
JIS:
bool(true)
ｱ
int(0)
ISO-2022-JP:
bool(false)
ｱ
int(0)

JIS X 0201 7bit kana with SO/SI
JIS:
bool(true)
ｱ
int(0)
ISO-2022-JP:
bool(false)
ｱ
int(0)

JIS X 0201 8bit kana
JIS:
bool(true)
ｱ
int(0)
ISO-2022-JP:
bool(false)
ｱ
int(0)

JIS X 0201 7bit kana with SO and ESC
JIS:
bool(false)
ｱ
int(0)
ISO-2022-JP:
bool(false)
ｱ
int(0)

JIS X 0201 7bit kana with ESC and SI
JIS:
bool(false)
ｱ
int(0)
ISO-2022-JP:
bool(false)
ｱ
int(0)

JIS X 0208 character
JIS:
bool(true)
鯛
int(0)
ISO-2022-JP:
bool(true)
鯛
int(0)

JIS X 0212 character
JIS:
bool(true)
鮋
int(0)
ISO-2022-JP:
bool(false)
鮋
int(0)

JIS X 0213 character
JIS:
bool(false)
?$(P}L
int(1)
ISO-2022-JP:
bool(false)
?$(P}L
int(2)

JIS C 6220-1969 ESC ( H
JIS:
bool(true)

int(2)
ISO-2022-JP:
bool(false)

int(2)

SO/SI when not in ASCII mode
JIS:
bool(false)

int(2)
ISO-2022-JP:
bool(false)

int(2)
