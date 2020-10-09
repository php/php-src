--TEST--
imap_fetchstructure() function : basic functionality
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/imap_include.inc');
$stream_id = setup_test_mailbox('', 1);

try {
    imap_fetchstructure($stream_id,0);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$z = imap_fetchstructure($stream_id,1);


$fields = array('type','encoding','ifsubtype','subtype',
'ifdescription','lines','bytes','parameters');

foreach ($fields as $key) {
    var_dump(isset($z->$key));
}
var_dump($z->type);
var_dump($z->encoding);
var_dump($z->bytes);
var_dump($z->lines);
var_dump($z->ifparameters);
var_dump(is_object($z->parameters[0]));

imap_close($stream_id);
?>
--CLEAN--
<?php
require_once('clean.inc');
?>
--EXPECTF--
Create a temporary mailbox and add 1 msgs
.. mailbox '{127.0.0.1:143/norsh}INBOX.phpttest' created
imap_fetchstructure(): Argument #2 ($message_num) must be greater than 0
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(%d)
int(%d)
int(%d)
int(%d)
int(1)
bool(true)
