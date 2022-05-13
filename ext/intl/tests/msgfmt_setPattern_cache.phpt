--TEST--
MessageFormatter::setPattern() invalidates arg types cache
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
//ini_set("intl.default_locale", "nl");

$mf = new MessageFormatter('en_US',
    "{0,number} -- {1,ordinal}");

var_dump($mf->format(array(1.3, 1.3)));
var_dump($mf->format(array(1.3, 1.3)));
$mf->setPattern("{0,ordinal} -- {1,number}");
var_dump($mf->format(array(1.3, 1.3)));

?>
--EXPECT--
string(10) "1.3 -- 1st"
string(10) "1.3 -- 1st"
string(10) "1st -- 1.3"
