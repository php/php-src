--TEST--
MessageFormatter::format() inconsistent types in named argument
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$fmt = <<<EOD
{foo,number} {foo}
EOD;

$mf = new MessageFormatter('en_US', $fmt);
var_dump($mf->format(array(7)));
?>
--EXPECTF--
Warning: MessageFormatter::format(): Inconsistent types declared for an argument in %s on line %d
bool(false)
