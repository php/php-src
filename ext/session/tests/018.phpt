--TEST--
rewriter correctly handles attribute names which contain dashes
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.use_only_cookies=0
session.use_strict_mode=0
session.cache_limiter=
session.use_trans_sid=1
session.name=PHPSESSID
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php

error_reporting(E_ALL);

session_id("test018");
session_start();
?>
<form accept-charset="ISO-8859-15, ISO-8859-1" action=url.php>
<?php
session_destroy();
?>
--EXPECT--
<form accept-charset="ISO-8859-15, ISO-8859-1" action=url.php><input type="hidden" name="PHPSESSID" value="test018" />
