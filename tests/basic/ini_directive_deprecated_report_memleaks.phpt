--TEST--
Deprecated INI directive report_memleaks warning
--INI--
report_memleaks=0
--FILE--
<?php
echo "Testing deprecated report_memleaks INI directive.\n";
?>
--EXPECT--
Deprecated: PHP Startup: Directive 'report_memleaks' is deprecated in Unknown on line 0
Testing deprecated report_memleaks INI directive.
