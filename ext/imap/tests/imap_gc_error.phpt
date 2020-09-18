--TEST--
imap_gc() incorrect parameter count
--CREDITS--
Paul Sohier
#phptestfest utrecht
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/imap_include.inc');
$stream_id = imap_open($default_mailbox, $username, $password) or
    die("Cannot connect to mailbox $default_mailbox: " . imap_last_error());
imap_gc($stream_id, -1);

?>
--EXPECTF--
Warning: imap_gc(): Invalid value for the flags parameter in %s on line %d
