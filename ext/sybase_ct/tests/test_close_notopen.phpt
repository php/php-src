--TEST--
Sybase-CT close not open
--SKIPIF--
<?php require('skipif.inc'); ?>
--FILE--
<?php
/* This file is part of PHP test framework for ext/sybase_ct
 *
 * $Id$ 
 */

  require('test.inc');

  sybase_close();
?>
--EXPECTF--

Warning: sybase_close(): Sybase:  No connection to close in %s on line %d
