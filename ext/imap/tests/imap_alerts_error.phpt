--TEST--
Test imap_alerts() function : error conditions
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : array imap_alerts(void)
 * Description: Returns an array of all IMAP alerts that have been generated since the last page load or since the last imap_alerts() call, whichever came last. The alert stack is cleared after imap_alerts() is called.
 * Source code: ext/imap/php_imap.c
 * Alias to functions:
 */

echo "*** Testing imap_alerts() : error conditions ***\n";

// One argument
echo "\n-- Testing imap_alerts() function with one argument --\n";
$extra_arg = 10;
var_dump( imap_alerts($extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing imap_alerts() : error conditions ***

-- Testing imap_alerts() function with one argument --

Warning: imap_alerts() expects exactly 0 parameters, 1 given in %s on line %d
NULL
===DONE===
