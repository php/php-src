--TEST--
Test output_add_rewrite_var() with and without nested session URL-Rewriting
--EXTENSIONS--
session
--INI--
session.trans_sid_tags="a=href,area=href,frame=src,form="
url_rewriter.tags="a=href,area=href,frame=src,form="
--FILE--
<?php
$testTags = <<<TEST

<a href=""></a>
<a href="./foo.php"></a>

<a href="//php.net/foo.php"></a>
<a href="http://php.net/foo.php"></a>
<a href="bad://php.net/foo.php"></a>
<a href="//www.php.net/foo.php"></a>

<a href="//session-trans-sid.com/foo.php"></a>
<a href="http://session-trans-sid.com/foo.php"></a>
<a href="bad://session-trans-sid.com/foo.php"></a>
<a href="//www.session-trans-sid.com/foo.php"></a>

<a href="//url-rewriter.com/foo.php"></a>
<a href="http://url-rewriter.com/foo.php"></a>
<a href="bad://url-rewriter.com/foo.php"></a>
<a href="//www.url-rewriter.com/foo.php"></a>

<form action="" method="get"> </form>
<form action="./foo.php" method="get"></form>

<form action="//php.net/foo.php" method="get"></form>
<form action="http://php.net/foo.php" method="get"></form>
<form action="bad://php.net/foo.php" method="get"></form>
<form action="//www.php.net/foo.php" method="get"></form>

<form action="//session-trans-sid.com/bar.php" method="get"></form>
<form action="http://session-trans-sid.com/bar.php" method="get"></form>
<form action="bad://session-trans-sid.com/bar.php" method="get"></form>
<form action="//www.session-trans-sid.com/bar.php" method="get"></form>

<form action="//url-rewriter.com/bar.php" method="get"></form>
<form action="http://url-rewriter.com/bar.php" method="get"></form>
<form action="bad://url-rewriter.com/bar.php" method="get"></form>
<form action="//www.url-rewriter.com/bar.php" method="get"></form>

TEST;

ob_start();

ini_set('session.trans_sid_hosts', 'session-trans-sid.com');
ini_set('url_rewriter.hosts', 'url-rewriter.com');

ini_set('session.use_only_cookies', 1);
ini_set('session.use_cookies', 1);
ini_set('session.use_strict_mode', 0);
ini_set('session.use_trans_sid', 0);

output_add_rewrite_var('<name>', '<value>');

echo "URL-Rewriting with output_add_rewrite_var() without transparent session id support\n";
echo $testTags;

ob_flush();

output_reset_rewrite_vars();

ini_set('session.use_only_cookies', 0);
ini_set('session.use_cookies', 0);
ini_set('session.use_strict_mode', 0);
ini_set('session.use_trans_sid', 1);

session_id('testid');
session_start();

output_add_rewrite_var('<NAME>', '<VALUE>');

echo "\nURL-Rewriting with transparent session id support without output_add_rewrite_var()\n";
echo $testTags;

ob_end_flush();


output_add_rewrite_var('<name2>', '<value2>');

echo "\nURL-Rewriting with output_add_rewrite_var() without transparent session id support\n";
echo $testTags;

--EXPECT--
URL-Rewriting with output_add_rewrite_var() without transparent session id support

<a href="?%3Cname%3E=%3Cvalue%3E"></a>
<a href="./foo.php?%3Cname%3E=%3Cvalue%3E"></a>

<a href="//php.net/foo.php"></a>
<a href="http://php.net/foo.php"></a>
<a href="bad://php.net/foo.php"></a>
<a href="//www.php.net/foo.php"></a>

<a href="//session-trans-sid.com/foo.php"></a>
<a href="http://session-trans-sid.com/foo.php"></a>
<a href="bad://session-trans-sid.com/foo.php"></a>
<a href="//www.session-trans-sid.com/foo.php"></a>

<a href="//url-rewriter.com/foo.php?%3Cname%3E=%3Cvalue%3E"></a>
<a href="http://url-rewriter.com/foo.php?%3Cname%3E=%3Cvalue%3E"></a>
<a href="bad://url-rewriter.com/foo.php"></a>
<a href="//www.url-rewriter.com/foo.php"></a>

<form action="" method="get"><input type="hidden" name="&lt;name&gt;" value="&lt;value&gt;" /> </form>
<form action="./foo.php" method="get"><input type="hidden" name="&lt;name&gt;" value="&lt;value&gt;" /></form>

<form action="//php.net/foo.php" method="get"></form>
<form action="http://php.net/foo.php" method="get"></form>
<form action="bad://php.net/foo.php" method="get"></form>
<form action="//www.php.net/foo.php" method="get"></form>

<form action="//session-trans-sid.com/bar.php" method="get"></form>
<form action="http://session-trans-sid.com/bar.php" method="get"></form>
<form action="bad://session-trans-sid.com/bar.php" method="get"></form>
<form action="//www.session-trans-sid.com/bar.php" method="get"></form>

<form action="//url-rewriter.com/bar.php" method="get"><input type="hidden" name="&lt;name&gt;" value="&lt;value&gt;" /></form>
<form action="http://url-rewriter.com/bar.php" method="get"><input type="hidden" name="&lt;name&gt;" value="&lt;value&gt;" /></form>
<form action="bad://url-rewriter.com/bar.php" method="get"></form>
<form action="//www.url-rewriter.com/bar.php" method="get"></form>

URL-Rewriting with transparent session id support without output_add_rewrite_var()

<a href="?PHPSESSID=testid&%3CNAME%3E=%3CVALUE%3E&%3Cname2%3E=%3Cvalue2%3E"></a>
<a href="./foo.php?PHPSESSID=testid&%3CNAME%3E=%3CVALUE%3E&%3Cname2%3E=%3Cvalue2%3E"></a>

<a href="//php.net/foo.php"></a>
<a href="http://php.net/foo.php"></a>
<a href="bad://php.net/foo.php"></a>
<a href="//www.php.net/foo.php"></a>

<a href="//session-trans-sid.com/foo.php?PHPSESSID=testid"></a>
<a href="http://session-trans-sid.com/foo.php?PHPSESSID=testid"></a>
<a href="bad://session-trans-sid.com/foo.php"></a>
<a href="//www.session-trans-sid.com/foo.php"></a>

<a href="//url-rewriter.com/foo.php?%3CNAME%3E=%3CVALUE%3E&%3Cname2%3E=%3Cvalue2%3E"></a>
<a href="http://url-rewriter.com/foo.php?%3CNAME%3E=%3CVALUE%3E&%3Cname2%3E=%3Cvalue2%3E"></a>
<a href="bad://url-rewriter.com/foo.php"></a>
<a href="//www.url-rewriter.com/foo.php"></a>

<form action="" method="get"><input type="hidden" name="&lt;NAME&gt;" value="&lt;VALUE&gt;" /><input type="hidden" name="&lt;name2&gt;" value="&lt;value2&gt;" /><input type="hidden" name="PHPSESSID" value="testid" /> </form>
<form action="./foo.php" method="get"><input type="hidden" name="&lt;NAME&gt;" value="&lt;VALUE&gt;" /><input type="hidden" name="&lt;name2&gt;" value="&lt;value2&gt;" /><input type="hidden" name="PHPSESSID" value="testid" /></form>

<form action="//php.net/foo.php" method="get"></form>
<form action="http://php.net/foo.php" method="get"></form>
<form action="bad://php.net/foo.php" method="get"></form>
<form action="//www.php.net/foo.php" method="get"></form>

<form action="//session-trans-sid.com/bar.php" method="get"><input type="hidden" name="PHPSESSID" value="testid" /></form>
<form action="http://session-trans-sid.com/bar.php" method="get"><input type="hidden" name="PHPSESSID" value="testid" /></form>
<form action="bad://session-trans-sid.com/bar.php" method="get"></form>
<form action="//www.session-trans-sid.com/bar.php" method="get"></form>

<form action="//url-rewriter.com/bar.php" method="get"><input type="hidden" name="&lt;NAME&gt;" value="&lt;VALUE&gt;" /><input type="hidden" name="&lt;name2&gt;" value="&lt;value2&gt;" /></form>
<form action="http://url-rewriter.com/bar.php" method="get"><input type="hidden" name="&lt;NAME&gt;" value="&lt;VALUE&gt;" /><input type="hidden" name="&lt;name2&gt;" value="&lt;value2&gt;" /></form>
<form action="bad://url-rewriter.com/bar.php" method="get"></form>
<form action="//www.url-rewriter.com/bar.php" method="get"></form>

URL-Rewriting with output_add_rewrite_var() without transparent session id support

<a href="?%3CNAME%3E=%3CVALUE%3E&%3Cname2%3E=%3Cvalue2%3E"></a>
<a href="./foo.php?%3CNAME%3E=%3CVALUE%3E&%3Cname2%3E=%3Cvalue2%3E"></a>

<a href="//php.net/foo.php"></a>
<a href="http://php.net/foo.php"></a>
<a href="bad://php.net/foo.php"></a>
<a href="//www.php.net/foo.php"></a>

<a href="//session-trans-sid.com/foo.php"></a>
<a href="http://session-trans-sid.com/foo.php"></a>
<a href="bad://session-trans-sid.com/foo.php"></a>
<a href="//www.session-trans-sid.com/foo.php"></a>

<a href="//url-rewriter.com/foo.php?%3CNAME%3E=%3CVALUE%3E&%3Cname2%3E=%3Cvalue2%3E"></a>
<a href="http://url-rewriter.com/foo.php?%3CNAME%3E=%3CVALUE%3E&%3Cname2%3E=%3Cvalue2%3E"></a>
<a href="bad://url-rewriter.com/foo.php"></a>
<a href="//www.url-rewriter.com/foo.php"></a>

<form action="" method="get"><input type="hidden" name="&lt;NAME&gt;" value="&lt;VALUE&gt;" /><input type="hidden" name="&lt;name2&gt;" value="&lt;value2&gt;" /> </form>
<form action="./foo.php" method="get"><input type="hidden" name="&lt;NAME&gt;" value="&lt;VALUE&gt;" /><input type="hidden" name="&lt;name2&gt;" value="&lt;value2&gt;" /></form>

<form action="//php.net/foo.php" method="get"></form>
<form action="http://php.net/foo.php" method="get"></form>
<form action="bad://php.net/foo.php" method="get"></form>
<form action="//www.php.net/foo.php" method="get"></form>

<form action="//session-trans-sid.com/bar.php" method="get"></form>
<form action="http://session-trans-sid.com/bar.php" method="get"></form>
<form action="bad://session-trans-sid.com/bar.php" method="get"></form>
<form action="//www.session-trans-sid.com/bar.php" method="get"></form>

<form action="//url-rewriter.com/bar.php" method="get"><input type="hidden" name="&lt;NAME&gt;" value="&lt;VALUE&gt;" /><input type="hidden" name="&lt;name2&gt;" value="&lt;value2&gt;" /></form>
<form action="http://url-rewriter.com/bar.php" method="get"><input type="hidden" name="&lt;NAME&gt;" value="&lt;VALUE&gt;" /><input type="hidden" name="&lt;name2&gt;" value="&lt;value2&gt;" /></form>
<form action="bad://url-rewriter.com/bar.php" method="get"></form>
<form action="//www.url-rewriter.com/bar.php" method="get"></form>
