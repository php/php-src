--TEST--
Deprecated INI directive report_memleaks/register_argc_argv warning
--INI--
register_argc_argv=1
report_memleaks=1
--FILE--
<?php
echo "Testing deprecated report_memleaks INI directive.\n";
?>
--EXPECT--
Deprecated: Directive 'report_memleaks' is deprecated in Unknown on line 0

Deprecated: Directive 'register_argc_argv' is deprecated in Unknown on line 0
Testing deprecated report_memleaks INI directive.
