--TEST--
RuleBasedBreakIterator::hashCode(): basic test
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");

$rbbi = BreakIterator::createWordInstance(NULL);
var_dump($rbbi->hashCode());

?>
==DONE==
--EXPECTF--
int(%d)
==DONE==