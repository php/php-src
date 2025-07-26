--TEST--
msgfmt_set_pattern() with syntax error
--EXTENSIONS--
intl
--FILE--
<?php

$locale = 'en_US';
$fmt = '{0,number} monkeys on {1,number} trees';
$broken = '{0,number} trees hosting {1,number monkeys';

$mf = new MessageFormatter($locale, $fmt);
$mf->setPattern($broken);
var_dump($mf->getErrorMessage());

msgfmt_set_pattern($mf, $broken);
var_dump($mf->getErrorMessage());

?>
--EXPECT--
string(71) "Error setting symbol value at line 0, offset 26: U_PATTERN_SYNTAX_ERROR"
string(71) "Error setting symbol value at line 0, offset 26: U_PATTERN_SYNTAX_ERROR"
