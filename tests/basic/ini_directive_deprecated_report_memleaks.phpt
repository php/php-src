--TEST--
Deprecated INI directive report_memleaks warning (debug build only)
--SKIPIF--
<?php
if (!ZEND_DEBUG_BUILD) die("skip requires debug build");
?>
--INI--
report_memleaks=0
--FILE--
<?php
echo "Testing deprecated report_memleaks INI directive.\n";
?>
--EXPECT--
Deprecated: PHP Startup: Directive 'report_memleaks' is deprecated in Unknown on line 0
Testing deprecated report_memleaks INI directive.
