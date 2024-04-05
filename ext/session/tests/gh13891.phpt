--TEST--
GH-13891 (memleak and segfault when using ini_set with session.trans_sid_hosts)
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
// We *must* set it here because the bug only triggers on a runtime edit
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
