--TEST--
Bug #50308 (session id not appended properly for empty anchor tags)
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.name=PHPSESSID
session.save_handler=files
session.use_trans_sid=1
session.use_only_cookies=0
--FILE--
<?php
@session_start();
?>
<a href=""/>
<a href="" />
<a href="foo"/>
<a href="foo" />
<a href=foo/>
<a href="/">
<a href=/>
<a href=?foo=bar/>
<a href="?foo=bar"/>
<a href=./>
<a href="./">
--EXPECTF--
Deprecated: PHP Startup: Disabling session.use_only_cookies INI setting is deprecated in Unknown on line 0

Deprecated: PHP Startup: Enabling session.use_trans_sid INI setting is deprecated in Unknown on line 0
<a href="?PHPSESSID=%s"/>
<a href="?PHPSESSID=%s" />
<a href="foo?PHPSESSID=%s"/>
<a href="foo?PHPSESSID=%s" />
<a href=foo/?PHPSESSID=%s>
<a href="/?PHPSESSID=%s">
<a href=/?PHPSESSID=%s>
<a href=?foo=bar/&PHPSESSID=%s>
<a href="?foo=bar&PHPSESSID=%s"/>
<a href=./?PHPSESSID=%s>
<a href="./?PHPSESSID=%s">
