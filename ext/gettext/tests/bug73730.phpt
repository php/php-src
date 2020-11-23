--TEST--
Bug #73730 (textdomain(null) throws in strict mode)
--SKIPIF--
<?php
if (!extension_loaded('gettext')) die('skip gettext extension is not available');
?>
--FILE--
<?php
declare(strict_types=1);

var_dump(textdomain(null));
?>
--EXPECT--
string(8) "messages"
