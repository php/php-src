--TEST--
imap_list() function : basic functionality
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/imap_include.inc');
$stream_id = imap_open($default_mailbox, $username, $password) or
    die("Cannot connect to mailbox $default_mailbox: " . imap_last_error());

imap_list($stream_id,$default_mailbox,'ezerz');


$z = imap_list($stream_id,$default_mailbox,'*');
var_dump(is_array($z));
var_dump($z[0]);

imap_close($stream_id);
?>
--EXPECTF--
bool(true)
string(%s) "{%s}%s"
