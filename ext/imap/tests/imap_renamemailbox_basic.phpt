--TEST--
imap_renamemailbox() function : basic functionality
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/setup/imap_include.inc');

$stream_id = setup_test_mailbox('imaprenamemailbox', 1);

if (!is_resource($stream_id)) {
    exit("TEST FAILED: Unable to create test mailbox\n");
}

$newbox = IMAP_DEFAULT_MAILBOX . "." . IMAP_MAILBOX_PHPT_PREFIX;

//commented because of bug #49901
//$ancError = error_reporting(0);
//$z = imap_renamemailbox($stream_id, $newbox.'not2', $newbox.'2');
//var_dump($z);
//error_reporting($ancError);
echo "Checking OK\n";


var_dump(imap_createmailbox($stream_id, $newbox.'.test'));
var_dump(imap_renamemailbox($stream_id, $newbox.'.test', $newbox.'.testd'));

imap_close($stream_id);
?>
--CLEAN--
<?php
$mailbox_suffix = 'imaprenamemailbox';
require_once('setup/clean.inc');
?>
--EXPECT--
Create a temporary mailbox and add 1 msgs
New mailbox created
Checking OK
bool(true)
bool(true)
