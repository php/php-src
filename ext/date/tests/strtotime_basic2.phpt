--TEST--
strtotime() with return false
--CREDITS--
"Anna Filina" <afilina@phpquebec.org>
#PHPTestFest2009 2009-05-02
--INI--
date.timezone=UTC
--FILE--
<?php
var_dump(strtotime('mayy 2 2009')); // misspelled month name
?>
--EXPECT--
bool(false)
