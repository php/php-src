--TEST--
Test basic function : variation3 use_trans_sid
--INI--
session.use_strict_mode=0
session.use_only_cookies=0
session.use_trans_sid=1
session.save_handler=files
session.hash_bits_per_character=4
session.hash_function=0
session.gc_probability=1
session.gc_divisor=1000
session.gc_maxlifetime=300
session.save_path=
session.name=PHPSESSID
url_rewriter.hosts=
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing basic session functionality : variation3 use_trans_sid ***\n";

/*
echo "*** test output_add_rewrite_var() ***\n";
output_add_rewrite_var('var', 'value');
echo '
<a href="/">test</a>
<a href="/#bar">test</a>
<a href="/?foo">test</a>
<a href="/?foo#bar">test</a>
<a href="/?foo=var">test</a>
<a href="/?foo=var#bar">test</a>
<a href="file.php">test</a>
<a href="file.php?foo">test</a>
<a href="file.php?foo=var">test</a>
<a href="http://php.net">test</a>
<a href="http://php.net/">test</a>
<a href="http://php.net/#bar">test</a>
<a href="http://php.net/?foo">test</a>
<a href="http://php.net/?foo#bar">test</a>
<a href="http://php.net/?foo=var">test</a>
<a href="http://php.net/?foo=var#bar">test</a>
<a href="http://php.net/file.php">test</a>
<a href="http://php.net/file.php#bar">test</a>
<a href="http://php.net/file.php?foo">test</a>
<a href="http://php.net/file.php?foo#bar">test</a>
<a href="http://php.net/file.php?foo=var">test</a>
<a href="http://php.net/file.php?foo=var#bar">test</a>
<a href="http://php.net/some/path/file.php">test</a>
<a href="http://php.net/some/path/file.php?foo">test</a>
<a href="http://php.net/some/path/file.php?foo=var">test</a>
<a href="http://php.net/some/path/file.php?foo=var#bar">test</a>
<a href="https://php.net">test</a>
<a href="https://php.net/">test</a>
<a href="https://php.net/?foo=var#bar">test</a>
<a href="https://php.net/file.php">test</a>
<a href="https://php.net/file.php?foo=var#bar">test</a>
<a href="https://php.net/some/path/file.php">test</a>
<a href="https://php.net/some/path/file.php?foo=var#bar">test</a>
<a href="https://php.net:8443">test</a>
<a href="https://php.net:8443/">test</a>
<a href="https://php.net:8443/?foo=var#bar">test</a>
<a href="https://php.net:8443/file.php">test</a>
<a href="https://php.net:8443/file.php?foo=var#bar">test</a>
<a href="https://php.net:8443/some/path/file.php">test</a>
<a href="https://php.net:8443/some/path/file.php?foo=var#bar">test</a>
<a href="//php.net">test</a>
<a href="//php.net/">test</a>
<a href="//php.net/#bar">test</a>
<a href="//php.net/?foo">test</a>
<a href="//php.net/?foo#bar">test</a>
<a href="//php.net/?foo=var">test</a>
<a href="//php.net/?foo=var#bar">test</a>
<a href="//php.net/file.php">test</a>
<a href="//php.net/file.php#bar">test</a>
<a href="//php.net/file.php?foo">test</a>
<a href="//php.net/file.php?foo#bar">test</a>
<a href="//php.net/file.php?foo=var">test</a>
<a href="//php.net/file.php?foo=var#bar">test</a>
<a href="//php.net/some/path/file.php">test</a>
<a href="//php.net/some/path/file.php?foo">test</a>
<a href="//php.net/some/path/file.php?foo=var">test</a>
<a href="//php.net/some/path/file.php?foo=var#bar">test</a>
<form action="script.php" method="post">
  <input type="text" name="test1"></input>
  <input type="text" name="test2" />
</form>
';
output_reset_rewrite_vars();
*/

echo "*** Test trans sid ***\n";
ob_start();
$session_id = 'session-basic3';
session_id($session_id);
session_start();
// Should add session ID to relative URL only for SECURITY
echo '
<a href="/">test</a>
<a href="/path">test</a>
<a href="/path/">test</a>
<a href="/path/?foo=var">test</a>
<a href="../">test</a>
<a href="../path">test</a>
<a href="../path/">test</a>
<a href="../path/?foo=var">test</a>

<a href="/#bar">test</a>
<a href="/path/#bar">test</a>
<a href="/path/?foo=var#bar">test</a>
<a href="../#bar">test</a>
<a href="../path/#bar">test</a>
<a href="../path/?foo=var#bar">test</a>

<a href="/?foo">test</a>
<a href="/?foo#bar">test</a>
<a href="/?foo=var">test</a>
<a href="/?foo=var#bar">test</a>
<a href="../?foo">test</a>
<a href="../?foo#bar">test</a>
<a href="../?foo=var">test</a>
<a href="../?foo=var#bar">test</a>

<a href="file.php">test</a>
<a href="file.php?foo">test</a>
<a href="file.php?foo=var">test</a>
<a href="file.php?foo=var#bar">test</a>
<a href="../file.php">test</a>
<a href="../file.php?foo">test</a>
<a href="../file.php?foo=var">test</a>
<a href="../file.php?foo=var#bar">test</a>

<a href="http://php.net">test</a>
<a href="http://php.net/">test</a>
<a href="http://php.net/#bar">test</a>
<a href="http://php.net/?foo">test</a>
<a href="http://php.net/?foo#bar">test</a>
<a href="http://php.net/?foo=var">test</a>
<a href="http://php.net/?foo=var#bar">test</a>
<a href="http://php.net/file.php">test</a>
<a href="http://php.net/file.php#bar">test</a>
<a href="http://php.net/file.php?foo">test</a>
<a href="http://php.net/file.php?foo#bar">test</a>
<a href="http://php.net/file.php?foo=var">test</a>
<a href="http://php.net/file.php?foo=var#bar">test</a>
<a href="http://php.net/some/path/file.php">test</a>
<a href="http://php.net/some/path/file.php?foo">test</a>
<a href="http://php.net/some/path/file.php?foo=var">test</a>
<a href="http://php.net/some/path/file.php?foo=var#bar">test</a>

<a href="https://php.net">test</a>
<a href="https://php.net/">test</a>
<a href="https://php.net/?foo=var#bar">test</a>
<a href="https://php.net/file.php">test</a>
<a href="https://php.net/file.php?foo=var#bar">test</a>
<a href="https://php.net/some/path/file.php">test</a>
<a href="https://php.net/some/path/file.php?foo=var#bar">test</a>
<a href="https://php.net:8443">test</a>
<a href="https://php.net:8443/">test</a>
<a href="https://php.net:8443/?foo=var#bar">test</a>
<a href="https://php.net:8443/file.php">test</a>
<a href="https://php.net:8443/file.php?foo=var#bar">test</a>
<a href="https://php.net:8443/some/path/file.php">test</a>
<a href="https://php.net:8443/some/path/file.php?foo=var#bar">test</a>

<a href="//php.net">test</a>
<a href="//php.net/">test</a>
<a href="//php.net/#bar">test</a>
<a href="//php.net/?foo">test</a>
<a href="//php.net/?foo#bar">test</a>
<a href="//php.net/?foo=var">test</a>
<a href="//php.net/?foo=var#bar">test</a>
<a href="//php.net/file.php">test</a>
<a href="//php.net/file.php#bar">test</a>
<a href="//php.net/file.php?foo">test</a>
<a href="//php.net/file.php?foo#bar">test</a>
<a href="//php.net/file.php?foo=var">test</a>
<a href="//php.net/file.php?foo=var#bar">test</a>
<a href="//php.net/some/path/file.php">test</a>
<a href="//php.net/some/path/file.php?foo">test</a>
<a href="//php.net/some/path/file.php?foo=var">test</a>
<a href="//php.net/some/path/file.php?foo=var#bar">test</a>

<form action="script.php" method="post">
  <input type="text" name="test1"></input>
  <input type="text" name="test2" />
</form>
<form action="../script.php" method="post">
  <input type="text" name="test1"></input>
  <input type="text" name="test2" />
</form>
<form action="/path/script.php" method="post">
  <input type="text" name="test1"></input>
  <input type="text" name="test2" />
</form>
<form action="../path/script.php" method="post">
  <input type="text" name="test1"></input>
  <input type="text" name="test2" />
</form>
<form method="post" action="http://php.net/script.php">
  <input type="text" name="test1"></input>
  <input type="text" name="test2" />
</form>
<form method="post" action="https://php.net/script.php">
  <input type="text" name="test1"></input>
  <input type="text" name="test2" />
</form>
<form method="post" action="//php.net/script.php">
  <input type="text" name="test1"></input>
  <input type="text" name="test2" />
</form>
';
var_dump(session_commit());

echo "*** Cleanup ***\n";
var_dump(session_start());
var_dump(session_id());
var_dump(session_destroy());

ob_end_flush();
?>
--EXPECT--
*** Testing basic session functionality : variation3 use_trans_sid ***
*** Test trans sid ***

<a href="/?PHPSESSID=session-basic3">test</a>
<a href="/path?PHPSESSID=session-basic3">test</a>
<a href="/path/?PHPSESSID=session-basic3">test</a>
<a href="/path/?foo=var&PHPSESSID=session-basic3">test</a>
<a href="../?PHPSESSID=session-basic3">test</a>
<a href="../path?PHPSESSID=session-basic3">test</a>
<a href="../path/?PHPSESSID=session-basic3">test</a>
<a href="../path/?foo=var&PHPSESSID=session-basic3">test</a>

<a href="/?PHPSESSID=session-basic3#bar">test</a>
<a href="/path/?PHPSESSID=session-basic3#bar">test</a>
<a href="/path/?foo=var&PHPSESSID=session-basic3#bar">test</a>
<a href="../?PHPSESSID=session-basic3#bar">test</a>
<a href="../path/?PHPSESSID=session-basic3#bar">test</a>
<a href="../path/?foo=var&PHPSESSID=session-basic3#bar">test</a>

<a href="/?foo&PHPSESSID=session-basic3">test</a>
<a href="/?foo&PHPSESSID=session-basic3#bar">test</a>
<a href="/?foo=var&PHPSESSID=session-basic3">test</a>
<a href="/?foo=var&PHPSESSID=session-basic3#bar">test</a>
<a href="../?foo&PHPSESSID=session-basic3">test</a>
<a href="../?foo&PHPSESSID=session-basic3#bar">test</a>
<a href="../?foo=var&PHPSESSID=session-basic3">test</a>
<a href="../?foo=var&PHPSESSID=session-basic3#bar">test</a>

<a href="file.php?PHPSESSID=session-basic3">test</a>
<a href="file.php?foo&PHPSESSID=session-basic3">test</a>
<a href="file.php?foo=var&PHPSESSID=session-basic3">test</a>
<a href="file.php?foo=var&PHPSESSID=session-basic3#bar">test</a>
<a href="../file.php?PHPSESSID=session-basic3">test</a>
<a href="../file.php?foo&PHPSESSID=session-basic3">test</a>
<a href="../file.php?foo=var&PHPSESSID=session-basic3">test</a>
<a href="../file.php?foo=var&PHPSESSID=session-basic3#bar">test</a>

<a href="http://php.net">test</a>
<a href="http://php.net/">test</a>
<a href="http://php.net/#bar">test</a>
<a href="http://php.net/?foo">test</a>
<a href="http://php.net/?foo#bar">test</a>
<a href="http://php.net/?foo=var">test</a>
<a href="http://php.net/?foo=var#bar">test</a>
<a href="http://php.net/file.php">test</a>
<a href="http://php.net/file.php#bar">test</a>
<a href="http://php.net/file.php?foo">test</a>
<a href="http://php.net/file.php?foo#bar">test</a>
<a href="http://php.net/file.php?foo=var">test</a>
<a href="http://php.net/file.php?foo=var#bar">test</a>
<a href="http://php.net/some/path/file.php">test</a>
<a href="http://php.net/some/path/file.php?foo">test</a>
<a href="http://php.net/some/path/file.php?foo=var">test</a>
<a href="http://php.net/some/path/file.php?foo=var#bar">test</a>

<a href="https://php.net">test</a>
<a href="https://php.net/">test</a>
<a href="https://php.net/?foo=var#bar">test</a>
<a href="https://php.net/file.php">test</a>
<a href="https://php.net/file.php?foo=var#bar">test</a>
<a href="https://php.net/some/path/file.php">test</a>
<a href="https://php.net/some/path/file.php?foo=var#bar">test</a>
<a href="https://php.net:8443">test</a>
<a href="https://php.net:8443/">test</a>
<a href="https://php.net:8443/?foo=var#bar">test</a>
<a href="https://php.net:8443/file.php">test</a>
<a href="https://php.net:8443/file.php?foo=var#bar">test</a>
<a href="https://php.net:8443/some/path/file.php">test</a>
<a href="https://php.net:8443/some/path/file.php?foo=var#bar">test</a>

<a href="//php.net">test</a>
<a href="//php.net/">test</a>
<a href="//php.net/#bar">test</a>
<a href="//php.net/?foo">test</a>
<a href="//php.net/?foo#bar">test</a>
<a href="//php.net/?foo=var">test</a>
<a href="//php.net/?foo=var#bar">test</a>
<a href="//php.net/file.php">test</a>
<a href="//php.net/file.php#bar">test</a>
<a href="//php.net/file.php?foo">test</a>
<a href="//php.net/file.php?foo#bar">test</a>
<a href="//php.net/file.php?foo=var">test</a>
<a href="//php.net/file.php?foo=var#bar">test</a>
<a href="//php.net/some/path/file.php">test</a>
<a href="//php.net/some/path/file.php?foo">test</a>
<a href="//php.net/some/path/file.php?foo=var">test</a>
<a href="//php.net/some/path/file.php?foo=var#bar">test</a>

<form action="script.php" method="post"><input type="hidden" name="PHPSESSID" value="session-basic3" />
  <input type="text" name="test1"></input>
  <input type="text" name="test2" />
</form>
<form action="../script.php" method="post"><input type="hidden" name="PHPSESSID" value="session-basic3" />
  <input type="text" name="test1"></input>
  <input type="text" name="test2" />
</form>
<form action="/path/script.php" method="post"><input type="hidden" name="PHPSESSID" value="session-basic3" />
  <input type="text" name="test1"></input>
  <input type="text" name="test2" />
</form>
<form action="../path/script.php" method="post"><input type="hidden" name="PHPSESSID" value="session-basic3" />
  <input type="text" name="test1"></input>
  <input type="text" name="test2" />
</form>
<form method="post" action="http://php.net/script.php">
  <input type="text" name="test1"></input>
  <input type="text" name="test2" />
</form>
<form method="post" action="https://php.net/script.php">
  <input type="text" name="test1"></input>
  <input type="text" name="test2" />
</form>
<form method="post" action="//php.net/script.php">
  <input type="text" name="test1"></input>
  <input type="text" name="test2" />
</form>
bool(true)
*** Cleanup ***
bool(true)
string(14) "session-basic3"
bool(true)
