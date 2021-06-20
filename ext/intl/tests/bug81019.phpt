--TEST--
Bug #81019: Unable to clone NumberFormatter after failed parse()
--EXTENSIONS--
intl
--FILE--
<?php

$fmt = new NumberFormatter('en_US', NumberFormatter::DECIMAL);
$fmt->parse('abc');
$fmt2 = clone $fmt;

$datefmt = new IntlDateFormatter('en_US', IntlDateFormatter::FULL, IntlDateFormatter::FULL);
$datefmt->parse('abc');
$datefmt2 = clone $datefmt;

$msgfmt = new MessageFormatter('en_US', '{0,number,integer}');
$msgfmt->parse('abc');
$msgfmt2 = clone $msgfmt;

?>
===DONE===
--EXPECT--
===DONE===
