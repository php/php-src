--TEST--
date_get_last_errors() return false
--CREDITS--
"Anna Filina" <afilina@phpquebec.org>
#PHPTestFest2009 2009-05-02
--INI--
date.timezone=UTC
--FILE--
<?php
var_dump(date_get_last_errors()); // no date was parsed, so no errors
?>
--EXPECT--
bool(false)
