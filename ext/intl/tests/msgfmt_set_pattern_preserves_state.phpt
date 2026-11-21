--TEST--
MessageFormatter::setPattern() preserves the previous pattern on failure
--CREDITS--
Wong Wai Kin
--EXTENSIONS--
intl
--FILE--
<?php
$formatter = new MessageFormatter('en', 'Hello, {0}!');
var_dump($formatter->format(['world']));

var_dump($formatter->setPattern('{0,date'));
var_dump($formatter->getPattern());
var_dump($formatter->format(['world']));

date_default_timezone_set('Asia/Tokyo');
var_dump($formatter->setPattern('{0,time,HH:mm}'));
var_dump($formatter->format([0]));
?>
--EXPECT--
string(13) "Hello, world!"
bool(false)
string(11) "Hello, {0}!"
string(13) "Hello, world!"
bool(true)
string(5) "09:00"
