--TEST--
GH-23094 NumberFormatter parse offsets use UTF-8 byte positions
--EXTENSIONS--
intl
--FILE--
<?php

$prefix = "\u{1F600}";

$formatter = new NumberFormatter('en_US', NumberFormatter::DECIMAL);
$offset = strlen($prefix);
var_dump($formatter->parse($prefix . '123', NumberFormatter::TYPE_INT32, $offset));
var_dump($offset);

$formatter = new NumberFormatter('en_US', NumberFormatter::CURRENCY);
$offset = strlen($prefix);
$currency = null;
var_dump($formatter->parseCurrency($prefix . '$123.45', $currency, $offset));
var_dump($currency);
var_dump($offset);

?>
--EXPECT--
int(123)
int(7)
float(123.45)
string(3) "USD"
int(11)
