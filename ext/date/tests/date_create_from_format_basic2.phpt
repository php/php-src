--TEST--
date_create_from_format() return false
--CREDITS--
"Anna Filina" <afilina@phpquebec.org>
#PHPTestFest2009 2009-05-02
--INI--
date.timezone=UTC
--FILE--
<?php
var_dump(date_create_from_format('Y-m-d', '2009---01')); // invalid date
?>
--EXPECT--
bool(false)
