--TEST--
Test imap_bodystruct() function : basic functionality
--EXTENSIONS--
imap
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php
echo "*** Testing string imap_bodystruct : basic functionality ***\n";
require_once(__DIR__.'/setup/imap_include.inc');

echo "Create a new mailbox for test and add a multipart msgs\n";
$imap_stream = setup_test_mailbox("imapbodystructbasic", 1, $mailbox, "multipart");

echo "\nGet and validate structure of body part 1\n";

$m = imap_bodystruct($imap_stream, 1, "1");

$mandatoryFields = [
    'ifsubtype',
    'ifdescription',
    'ifid',
    'ifdisposition',
    'ifdparameters',
    'ifparameters',
];

function isValid($param) {
    return ($param == 0) || ($param == 1);
}

foreach($mandatoryFields as $mf) {
    if (isValid($m->$mf)) {
        echo "$mf is 0 or 1\n";
    } else {
        echo "$mf FAIL\n";
    }
}

if(is_array($m->parameters)) {
    echo "parameters is an array\n";
}

echo "\nTry to get part 4!\n";
var_dump(imap_bodystruct($imap_stream, 1, "4"));

imap_close($imap_stream);

?>
--CLEAN--
<?php
$mailbox_suffix = 'imapbodystructbasic';
require_once(__DIR__ . '/setup/clean.inc');
?>
--EXPECT--
*** Testing string imap_bodystruct : basic functionality ***
Create a new mailbox for test and add a multipart msgs
Create a temporary mailbox and add 1 msgs
New mailbox created

Get and validate structure of body part 1
ifsubtype is 0 or 1
ifdescription is 0 or 1
ifid is 0 or 1
ifdisposition is 0 or 1
ifdparameters is 0 or 1
ifparameters is 0 or 1
parameters is an array

Try to get part 4!
bool(false)
