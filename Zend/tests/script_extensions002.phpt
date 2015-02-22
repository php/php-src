--TEST--
zend.script_extensions under CGI
--INI--
display_errors=On
display_startup_errors=On
--SKIPIF--
<?php
if (strstr(PHP_SAPI, 'cgi') == FALSE) {
  die('Skip - CGI only test');
}
--FILE--
<?php
include_once('script_extensions-dummy1.php');
require_once('script_extensions-dummy2.php');
include('script_extensions-dummy1.inc');
require('script_extensions-dummy2.inc');
include_once('script_extensions-dummy1.inc');
require_once('script_extensions-dummy2.inc');
echo "Done\n";
?>
--EXPECTF--
X-Powered-By: PHP/%s-%s
Content-type: text/html; charset=UTF-8

%sscript_extensions-dummy1.php
%sscript_extensions-dummy2.php

Fatal error: Illegal script extension detected in %s on line %d