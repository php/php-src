--TEST--
tidy_get_opt_doc()
--SKIPIF--
<?php if (!extension_loaded("tidy") || !function_exists('tidy_get_opt_doc')) print "skip"; ?>
--FILE--
<?php

try {
    tidy_get_opt_doc(new tidy, 'some_bogus_cfg');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

$t = new tidy;
var_dump($t->getOptDoc('ncr'));
var_dump(strlen(tidy_get_opt_doc($t, 'wrap')) > 99);
?>
--EXPECT--
tidy_get_opt_doc(): Argument #2 ($option) is an invalid configuration option, "some_bogus_cfg" given
string(73) "This option specifies if Tidy should allow numeric character references. "
bool(true)
