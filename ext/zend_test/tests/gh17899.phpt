--TEST--
GH-17899 (zend_test_compile_string with opcache crash on invalid script path)
--EXTENSIONS--
zend_test
--INI--
opcache.enable_cli=1
--CREDITS--
YuanchengJiang
--FILE--
<?php
$source = '<?php
require("sumfile.php");
?>';
try {zend_test_compile_string($source,$source,$c);} catch (Exception $e) { echo($e); }
?>
--EXPECTF--

Warning: Undefined variable $c in %s on line %d

Deprecated: zend_test_compile_string(): Passing null to parameter #3 ($position) of type int is deprecated in %s on line %d

Warning: require(sumfile.php): Failed to open stream: No such file or directory in <?php
require("sumfile.php");
?> on line %d

Fatal error: Uncaught Error: Failed opening required 'sumfile.php' (include_path='.%s') in <?php
require("sumfile.php");
?>:%d
Stack trace:
#0 %s(%d): zend_test_compile_string('<?php\nrequire("...', '<?php\nrequire("...', NULL)
#1 {main}
  thrown in <?php
require("sumfile.php");
?> on line %d

