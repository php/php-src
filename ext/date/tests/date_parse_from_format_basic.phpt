--TEST--
date_parse_from_format() return false with wrong params
--CREDITS--
"Anna Filina" <afilina@phpquebec.org>
#PHPTestFest2009 2009-05-02
--INI--
date.timezone=UTC
display_errors=false
--FILE--
<?php
var_dump(date_parse_from_format()); // invalid parameters
?>
--EXPECT--
bool(false)