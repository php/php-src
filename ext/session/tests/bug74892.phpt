--TEST--
Bug #74892 Url Rewriting (trans_sid) not working on urls that start with #
--INI--
session.use_cookies=0
session.use_only_cookies=0
session.use_trans_sid=1
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
ob_start();
ini_set('session.trans_sid_hosts','php.net'); // This value cannot be set in the INI file
session_id('sessionidhere');
session_start();

?>
<p><a href="index.php">Click This Anchor Tag!</a></p>
<p><a href="index.php#place">External link with anchor</a></p>
<p><a href="http://php.net#foo">External link with anchor 2</a></p>
<p><a href="#place">Internal link</a></p>
--EXPECTF--
Deprecated: PHP Startup: Disabling session.use_only_cookies INI setting is deprecated in Unknown on line 0

Deprecated: PHP Startup: Enabling session.use_trans_sid INI setting is deprecated in Unknown on line 0

Deprecated: ini_set(): Usage of session.trans_sid_hosts INI setting is deprecated in %s on line 3
<p><a href="index.php?PHPSESSID=sessionidhere">Click This Anchor Tag!</a></p>
<p><a href="index.php?PHPSESSID=sessionidhere#place">External link with anchor</a></p>
<p><a href="http://php.net?PHPSESSID=sessionidhere#foo">External link with anchor 2</a></p>
<p><a href="#place">Internal link</a></p>
