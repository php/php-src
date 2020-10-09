--TEST--
imap_getsubscribed() function : basic functionality
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
if (getenv("SKIP_ASAN")) die("skip asan chokes on this: 'LeakSanitizer does not work under ptrace (strace, gdb, etc)'");
?>
--FILE--
<?php

require_once(__DIR__.'/imap_include.inc');
$stream_id = imap_open($default_mailbox, $username, $password) or
    die("Cannot connect to mailbox $default_mailbox: " . imap_last_error());

var_dump(imap_getsubscribed($stream_id,$default_mailbox,'ezDvfXvbvcxSerz'));


echo "Checking OK\n";

$newbox = $default_mailbox . "." . $mailbox_prefix;

imap_createmailbox($stream_id, $newbox);
imap_subscribe($stream_id, $newbox);

$z = imap_getsubscribed($stream_id,$default_mailbox,'*');

var_dump(is_array($z));
var_dump($z[0]);

imap_close($stream_id);
?>
--CLEAN--
<?php
require_once('clean.inc');
?>
--EXPECTF--
bool(false)
Checking OK
bool(true)
object(stdClass)#%d (%d) {
  [%sname"]=>
  string(%d) "{%s}%s"
  [%sattributes"]=>
  int(%d)
  [%sdelimiter"]=>
  string(%d) "%s"
}
