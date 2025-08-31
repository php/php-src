--TEST--
Bug GH-8364 (msgfmt_format $values may not support references)
--EXTENSIONS--
intl
--FILE--
<?php
$formatter = new MessageFormatter('en', 'translate {0}');
$args = ['string', 'string'];
foreach ($args as &$arg) {
//     do nothing;
}
$result = $formatter->format($args);
var_dump($result);
var_dump(intl_get_error_code());
?>
--EXPECT--
string(16) "translate string"
int(0)
