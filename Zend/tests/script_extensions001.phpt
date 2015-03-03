--TEST--
zend.script_extensions under CLI
--INI--
display_errors=Off
--SKIPIF--
<?php
if (PHP_SAPI != 'cli') {
  die('Skip - CLI only test');
}
--FILE--
<?php
include('script_extensions-dummy1.inc');
include_once('script_extensions-dummy1.inc');
include_once('script_extensions-dummy1.php');
require('script_extensions-dummy2.inc');
require_once('script_extensions-dummy2.inc');
require_once('script_extensions-dummy2.php');
echo "Done\n";
?>
--EXPECTF--
%sscript_extensions-dummy1.inc
%sscript_extensions-dummy1.php
%sscript_extensions-dummy2.inc
%sscript_extensions-dummy2.php
Done
