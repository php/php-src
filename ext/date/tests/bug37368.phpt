--TEST--
Bug #37368 (Incorrect timestamp returned for strtotime()).
--FILE--
<?php
date_default_timezone_set('UTC');
echo date("r", strtotime("Mon, 08 May 2006 13:06:44 -0400 +30 days"));
?>
--EXPECT--
Wed, 07 Jun 2006 17:06:44 +0000
