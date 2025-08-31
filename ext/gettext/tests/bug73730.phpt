--TEST--
Bug #73730 (textdomain(null) throws in strict mode)
--EXTENSIONS--
gettext
--FILE--
<?php
declare(strict_types=1);

var_dump(textdomain(null));
?>
--EXPECT--
string(8) "messages"
