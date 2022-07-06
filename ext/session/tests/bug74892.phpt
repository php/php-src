--TEST--
Bug #74892 Url Rewriting (trans_sid) not working on urls that start with #
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
ini_set('session.use_cookies', '0');
ini_set('session.use_only_cookies',0);
ini_set('session.use_trans_sid',1);
ini_set('session.trans_sid_hosts','php.net');
session_id('sessionidhere');
session_start();

?>
<p><a href="index.php">Click This Anchor Tag!</a></p>
<p><a href="index.php#place">External link with anchor</a></p>
<p><a href="http://php.net#foo">External link with anchor 2</a></p>
<p><a href="#place">Internal link</a></p>
--EXPECT--
<p><a href="index.php?PHPSESSID=sessionidhere">Click This Anchor Tag!</a></p>
<p><a href="index.php?PHPSESSID=sessionidhere#place">External link with anchor</a></p>
<p><a href="http://php.net?PHPSESSID=sessionidhere#foo">External link with anchor 2</a></p>
<p><a href="#place">Internal link</a></p>
