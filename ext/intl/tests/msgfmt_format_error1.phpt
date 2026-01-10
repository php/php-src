--TEST--
MessageFormatter::format() insufficient numeric arguments
--EXTENSIONS--
intl
--INI--
intl.use_exceptions=On
--FILE--
<?php

$fmt = <<<EOD
{0} {1}
EOD;

$mf = new MessageFormatter('en_US', $fmt);
var_dump($mf->format(array(7)));
?>
--EXPECT--
string(5) "7 {1}"
