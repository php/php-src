--TEST--
filter_var() supports all Filter\Filter and Filter\Flag enum cases
--EXTENSIONS--
filter
--FILE--
<?php

use Filter\Filter;
use Filter\Flag;

echo "Filter enum cases:\n";

foreach (Filter::cases() as $filter) {
    var_dump($filter->name, $filter->value);
}

echo "\nFlag enum cases:\n";

foreach (Flag::cases() as $flag) {
    var_dump($flag->name, $flag->value);
}

echo "\nFilter enum as filter argument:\n";

var_dump(filter_var('123', Filter::DEFAULT));
var_dump(filter_var('123', Filter::VALIDATE_BOOL));
var_dump(filter_var('123', Filter::VALIDATE_INT));
var_dump(filter_var('123.45', Filter::VALIDATE_FLOAT));
var_dump(filter_var('example.com', Filter::VALIDATE_DOMAIN));
var_dump(filter_var('https://example.com', Filter::VALIDATE_URL));
var_dump(filter_var('test@example.com', Filter::VALIDATE_EMAIL));
var_dump(filter_var('127.0.0.1', Filter::VALIDATE_IP));
var_dump(filter_var('00:11:22:33:44:55', Filter::VALIDATE_MAC));

echo "\nFlag enum as options argument:\n";

var_dump(filter_var('0xff', Filter::VALIDATE_INT, Flag::ALLOW_HEX));
var_dump(filter_var('077', Filter::VALIDATE_INT, Flag::ALLOW_OCTAL));
var_dump(filter_var('1.5', Filter::VALIDATE_FLOAT, Flag::ALLOW_FRACTION));
var_dump(filter_var('1,000', Filter::VALIDATE_FLOAT, Flag::ALLOW_THOUSAND));
var_dump(filter_var('1e3', Filter::VALIDATE_FLOAT, Flag::ALLOW_SCIENTIFIC));

echo "\nFlag enum in associative options:\n";

var_dump(filter_var('0xff', Filter::VALIDATE_INT, [
    'flags' => Flag::ALLOW_HEX,
]));

var_dump(filter_var('077', Filter::VALIDATE_INT, [
    'flags' => Flag::ALLOW_OCTAL,
]));

var_dump(filter_var('123', Filter::VALIDATE_INT, [
    'flags' => Flag::NONE,
    'options' => [
        'min_range' => 100,
        'max_range' => 200,
    ],
]));

echo "\nFlag backing value:\n";

var_dump(filter_var('0xff', Filter::VALIDATE_INT, [
    'flags' => Flag::ALLOW_HEX->value,
]));

echo "\nLegacy string flags:\n";

var_dump(filter_var('0xff', Filter::VALIDATE_INT, [
    'flags' => (string) FILTER_FLAG_ALLOW_HEX,
]));

echo "\nCombined flags:\n";

var_dump(filter_var(
    '0xff',
    Filter::VALIDATE_INT,
    Flag::ALLOW_HEX->value | Flag::ALLOW_THOUSAND->value
));

echo "\nFilter enum backing value:\n";

var_dump(filter_var('123', Filter::VALIDATE_INT->value));

echo "\nInvalid enum combinations:\n";

try {
    filter_var('123', Filter::VALIDATE_INT, Filter::DEFAULT);
} catch (Throwable $e) {
    var_dump(get_class($e), $e->getMessage());
}
?>
--EXPECT--
Filter enum cases:
string(7) "DEFAULT"
int(516)
string(13) "VALIDATE_BOOL"
int(258)
string(12) "VALIDATE_INT"
int(257)
string(14) "VALIDATE_FLOAT"
int(259)
string(15) "VALIDATE_REGEXP"
int(272)
string(15) "VALIDATE_DOMAIN"
int(277)
string(12) "VALIDATE_URL"
int(273)
string(14) "VALIDATE_EMAIL"
int(274)
string(11) "VALIDATE_IP"
int(275)
string(12) "VALIDATE_MAC"
int(276)
string(15) "SANITIZE_STRING"
int(513)
string(14) "SANITIZE_EMAIL"
int(517)
string(12) "SANITIZE_URL"
int(518)
string(19) "SANITIZE_NUMBER_INT"
int(519)
string(21) "SANITIZE_NUMBER_FLOAT"
int(520)
string(22) "SANITIZE_SPECIAL_CHARS"
int(515)
string(27) "SANITIZE_FULL_SPECIAL_CHARS"
int(522)
string(20) "SANITIZE_ADD_SLASHES"
int(523)
string(8) "CALLBACK"
int(1024)

Flag enum cases:
string(4) "NONE"
int(0)
string(11) "ALLOW_OCTAL"
int(1)
string(9) "ALLOW_HEX"
int(2)
string(9) "STRIP_LOW"
int(4)
string(10) "STRIP_HIGH"
int(8)
string(14) "STRIP_BACKTICK"
int(512)
string(10) "ENCODE_LOW"
int(16)
string(11) "ENCODE_HIGH"
int(32)
string(10) "ENCODE_AMP"
int(64)
string(16) "NO_ENCODE_QUOTES"
int(128)
string(17) "EMPTY_STRING_NULL"
int(256)
string(14) "ALLOW_FRACTION"
int(4096)
string(14) "ALLOW_THOUSAND"
int(8192)
string(16) "ALLOW_SCIENTIFIC"
int(16384)
string(13) "PATH_REQUIRED"
int(262144)
string(14) "QUERY_REQUIRED"
int(524288)
string(4) "IPV4"
int(1048576)
string(4) "IPV6"
int(2097152)
string(12) "NO_RES_RANGE"
int(4194304)
string(13) "NO_PRIV_RANGE"
int(8388608)
string(12) "GLOBAL_RANGE"
int(536870912)

Filter enum as filter argument:
string(3) "123"
bool(false)
int(123)
float(123.45)
string(11) "example.com"
string(19) "https://example.com"
string(16) "test@example.com"
string(9) "127.0.0.1"
string(17) "00:11:22:33:44:55"

Flag enum as options argument:
int(255)
int(63)
float(1.5)
float(1000)
float(1000)

Flag enum in associative options:
int(255)
int(63)
int(123)

Flag backing value:
int(255)

Legacy string flags:
int(255)

Combined flags:
int(255)

Filter enum backing value:
int(123)

Invalid enum combinations:
string(9) "TypeError"
string(88) "filter_var(): Argument #3 ($options) must be of type array|enum|int, Filter\Filter given"
