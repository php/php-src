--TEST--
tidy_get_opt_doc()
--SKIPIF--
<?php if (!extension_loaded("tidy") || !function_exists('tidy_get_opt_doc')) print "skip"; ?>
--FILE--
<?php

var_dump(tidy_get_opt_doc(new tidy, 'some_bogus_cfg'));

$t = new tidy;
var_dump($t->getOptDoc('ncr'));
var_dump(strlen(tidy_get_opt_doc($t, 'wrap')) > 99);
?>
--EXPECTF--
Warning: tidy_get_opt_doc(): Unknown Tidy Configuration Option 'some_bogus_cfg' in %s021.php on line 3
bool(false)
string(73) "This option specifies if Tidy should allow numeric character references. "
bool(true)
