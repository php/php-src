--TEST--
IntlDateFormatter parsing uses UTF-8 byte offsets
--EXTENSIONS--
intl
--FILE--
<?php
$formatter = new IntlDateFormatter('en_US', IntlDateFormatter::NONE, IntlDateFormatter::NONE, 'UTC', IntlDateFormatter::GREGORIAN, 'yyyy-MM-dd');
$prefix = "\u{1F600}";
$text = $prefix . '2017-10-12 tail';

echo "parse():\n";
$offset = strlen($prefix);
var_dump(gmdate('Y-m-d', $formatter->parse($text, $offset)), $offset);

echo "localtime():\n";
$offset = strlen($prefix);
$tm = $formatter->localtime($text, $offset);
var_dump($tm['tm_year'] + 1900, $tm['tm_mon'] + 1, $tm['tm_mday'], $offset);

echo "parseToCalendar():\n";
$offset = strlen($prefix);
var_dump(gmdate('Y-m-d', $formatter->parseToCalendar($text, $offset)), $offset);

echo "failed parse:\n";
$offset = strlen($prefix);
var_dump($formatter->parse($prefix . '2017-xx-12', $offset), $offset);

echo "offset inside a character:\n";
$offset = 1;
var_dump($formatter->parse($text, $offset), $offset, intl_get_error_message());

echo "partial match inside a surrogate pair:\n";
$formatter = new IntlDateFormatter('en_US', IntlDateFormatter::NONE, IntlDateFormatter::NONE, 'UTC', IntlDateFormatter::GREGORIAN, "\u{20AC}\u{1F600}yyyy-MM-dd");
$offset = 0;
var_dump($formatter->parse("\u{20AC}\u{1F601}2017-10-12", $offset), $offset);
$formatter = new IntlDateFormatter('en_US', IntlDateFormatter::NONE, IntlDateFormatter::NONE, 'UTC', IntlDateFormatter::GREGORIAN, "yyyy-MM-dd\u{1F600}");
foreach (['parse', 'localtime', 'parseToCalendar'] as $method) {
    $offset = 0;
    $formatter->$method("2017-10-12\u{1F601}", $offset);
    echo "$method(): ";
    var_dump($offset);
}
?>
--EXPECT--
parse():
string(10) "2017-10-12"
int(14)
localtime():
int(2017)
int(10)
int(12)
int(14)
parseToCalendar():
string(10) "2017-10-12"
int(14)
failed parse:
bool(false)
int(9)
offset inside a character:
bool(false)
int(1)
string(70) "IntlDateFormatter::parse(): Invalid UTF-8 offset: U_INVALID_CHAR_FOUND"
partial match inside a surrogate pair:
bool(false)
int(3)
parse(): int(10)
localtime(): int(10)
parseToCalendar(): int(10)
