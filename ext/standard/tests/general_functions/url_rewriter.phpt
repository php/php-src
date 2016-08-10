--TEST--
URL Rewriter tests
--INI--
url_rewriter.tags="a=href,form="
session.use_only_cookies=0
session.use_trans_sid=1
session.use_strict_mode=0
--FILE--
<?php
session_id('id');

$_SERVER['HTTP_HOST'] = 'php.net';
session_start();
output_add_rewrite_var('a','b');
?>

<a></a>
<a href=""></a>
<a href="foo"></a>
<a href="?foo"></a>
<a href="/foo"></a>
<a href="foo=bar"></a>
<a href="foo.php#bar"></a>
<a href="../foo.php#bar"></a>

<a href="//bad.net/foo"></a>
<a href="//bad.net/?foo"></a>
<a href="//bad.net/foo"></a>
<a href="//bad.net/foo=bar"></a>
<a href="//bad.net/foo.php#bar"></a>
<a href="//bad.net/../foo.php#bar"></a>

<a href="//php.net/foo"></a>
<a href="//php.net/?foo"></a>
<a href="//php.net//foo"></a>
<a href="//php.net/foo=bar"></a>
<a href="//php.net/foo.php#bar"></a>

<a href="http://bad.net/foo"></a>
<a href="http://bad.net/?foo"></a>
<a href="http://bad.net/foo"></a>
<a href="http://bad.net/foo=bar"></a>
<a href="http://bad.net/foo.php#bar"></a>
<a href="http://bad.net/../foo.php#bar"></a>

<a href="http://php.net/foo"></a>
<a href="http://php.net/?foo"></a>
<a href="http://php.net//foo"></a>
<a href="http://php.net/foo=bar"></a>
<a href="http://php.net/foo.php#bar"></a>
<a href="http://php.net/../foo.php#bar"></a>

<a href="bad://bad.net/foo"></a>
<a href="bad://bad.net/?foo"></a>
<a href="bad://bad.net/foo"></a>
<a href="bad://bad.net/foo=bar"></a>
<a href="bad://bad.net/foo.php#bar"></a>
<a href="bad://bad.net/../foo.php#bar"></a>

<a href="bad://php.net/foo"></a>
<a href="bad://php.net/?foo"></a>
<a href="bad://php.net//foo"></a>
<a href="bad://php.net/foo=bar"></a>
<a href="bad://php.net/foo.php#bar"></a>
<a href="bad://php.net/../foo.php#bar"></a>

<form></form>
<form action=""></form>
<form action="foo.php"></form>
<form action="//php.net/foo.php"></form>
<form action="http://php.net/foo.php"></form>

<form action="bad://php.net/foo.php"></form>
<form action="//bad.net/foo.php"></form>
<form action="http://php.net/foo.php"></form>
<form action="bad://php.net/foo.php"></form>
<form action="//bad.net/foo.php"></form>
--EXPECT--
<a></a>
<a href="?PHPSESSID=id&a=b"></a>
<a href="foo?PHPSESSID=id&a=b"></a>
<a href="?foo&PHPSESSID=id&a=b"></a>
<a href="/foo?PHPSESSID=id&a=b"></a>
<a href="foo=bar?PHPSESSID=id&a=b"></a>
<a href="foo.php?PHPSESSID=id&a=b#bar"></a>
<a href="../foo.php?PHPSESSID=id&a=b#bar"></a>

<a href="//bad.net/foo"></a>
<a href="//bad.net/?foo"></a>
<a href="//bad.net/foo"></a>
<a href="//bad.net/foo=bar"></a>
<a href="//bad.net/foo.php#bar"></a>
<a href="//bad.net/../foo.php#bar"></a>

<a href="//php.net/foo?PHPSESSID=id&a=b"></a>
<a href="//php.net/?foo&PHPSESSID=id&a=b"></a>
<a href="//php.net//foo?PHPSESSID=id&a=b"></a>
<a href="//php.net/foo=bar?PHPSESSID=id&a=b"></a>
<a href="//php.net/foo.php?PHPSESSID=id&a=b#bar"></a>

<a href="http://bad.net/foo"></a>
<a href="http://bad.net/?foo"></a>
<a href="http://bad.net/foo"></a>
<a href="http://bad.net/foo=bar"></a>
<a href="http://bad.net/foo.php#bar"></a>
<a href="http://bad.net/../foo.php#bar"></a>

<a href="http://php.net/foo"></a>
<a href="http://php.net/?foo"></a>
<a href="http://php.net//foo"></a>
<a href="http://php.net/foo=bar"></a>
<a href="http://php.net/foo.php#bar"></a>
<a href="http://php.net/../foo.php#bar"></a>

<a href="bad://bad.net/foo"></a>
<a href="bad://bad.net/?foo"></a>
<a href="bad://bad.net/foo"></a>
<a href="bad://bad.net/foo=bar"></a>
<a href="bad://bad.net/foo.php#bar"></a>
<a href="bad://bad.net/../foo.php#bar"></a>

<a href="bad://php.net/foo"></a>
<a href="bad://php.net/?foo"></a>
<a href="bad://php.net//foo"></a>
<a href="bad://php.net/foo=bar"></a>
<a href="bad://php.net/foo.php#bar"></a>
<a href="bad://php.net/../foo.php#bar"></a>

<form><input type="hidden" name="PHPSESSID" value="id" /><input type="hidden" name="a" value="b" /></form>
<form action=""><input type="hidden" name="PHPSESSID" value="id" /><input type="hidden" name="a" value="b" /></form>
<form action="foo.php"><input type="hidden" name="PHPSESSID" value="id" /><input type="hidden" name="a" value="b" /></form>
<form action="//php.net/foo.php"><input type="hidden" name="PHPSESSID" value="id" /><input type="hidden" name="a" value="b" /></form>
<form action="http://php.net/foo.php"><input type="hidden" name="PHPSESSID" value="id" /><input type="hidden" name="a" value="b" /></form>

<form action="bad://php.net/foo.php"><input type="hidden" name="PHPSESSID" value="id" /><input type="hidden" name="a" value="b" /></form>
<form action="//bad.net/foo.php"><input type="hidden" name="PHPSESSID" value="id" /><input type="hidden" name="a" value="b" /></form>
<form action="http://php.net/foo.php"><input type="hidden" name="PHPSESSID" value="id" /><input type="hidden" name="a" value="b" /></form>
<form action="bad://php.net/foo.php"><input type="hidden" name="PHPSESSID" value="id" /><input type="hidden" name="a" value="b" /></form>
<form action="//bad.net/foo.php"><input type="hidden" name="PHPSESSID" value="id" /><input type="hidden" name="a" value="b" /></form>
