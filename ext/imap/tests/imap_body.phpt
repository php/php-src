--TEST--
imap_body() incorrect parameter count
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

imap_body($stream_id,-1);
imap_body($stream_id,1,-1);

//Access not existing
var_dump(imap_body($stream_id, 999, FT_UID));

imap_close($stream_id);

?>
--EXPECTF--
Warning: imap_body(): Bad message number in %s on line %d

Warning: imap_body(): Invalid value for the options parameter in %s on line %d

Warning: imap_body(): Bad message number in %s on line %d
bool(false)
