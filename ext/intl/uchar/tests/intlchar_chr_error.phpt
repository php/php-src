--TEST--
IntlChar wrong parameters passed
--SKIPIF--
<?php if (!extension_loaded('intl')) die("skip requires ext/intl") ?>
--FILE--
<?php
IntlChar::chr();
--EXPECTF--
Warning: IntlChar::chr() expects exactly 1 parameter, 0 given in %s on line %d