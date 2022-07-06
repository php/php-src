--TEST--
rewriter handles form and fieldset tags correctly
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.use_only_cookies=0
session.use_strict_mode=0
session.cache_limiter=
session.use_trans_sid=1
url_rewriter.tags="a=href,area=href,frame=src,input=src,form=,fieldset="
session.name=PHPSESSID
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php

error_reporting(E_ALL);
ini_set('session.trans_sid_hosts', 'php.net');
$_SERVER['HTTP_HOST'] = 'php.net';

session_id("abtest");
session_start();
?>
<form action="//bad.net/do.php">
<fieldset>
<form action="//php.net/do.php">
<fieldset>
<?php

ob_flush();

ini_set("url_rewriter.tags", "a=href,area=href,frame=src,input=src,form=");

?>
<form action="../do.php">
<fieldset>
<?php

ob_flush();

ini_set("url_rewriter.tags", "a=href,area=href,frame=src,input=src,form=fakeentry");

?>
<form action="/do.php">
<fieldset>
<?php

ob_flush();

ini_set("url_rewriter.tags", "a=href,fieldset=,area=href,frame=src,input=src");

?>
<form action="/foo/do.php">
<fieldset>
<?php

session_destroy();
?>
--EXPECT--
<form action="//bad.net/do.php">
<fieldset>
<form action="//php.net/do.php"><input type="hidden" name="PHPSESSID" value="abtest" />
<fieldset>
<form action="../do.php"><input type="hidden" name="PHPSESSID" value="abtest" />
<fieldset>
<form action="/do.php"><input type="hidden" name="PHPSESSID" value="abtest" />
<fieldset>
<form action="/foo/do.php"><input type="hidden" name="PHPSESSID" value="abtest" />
<fieldset>
