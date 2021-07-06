--TEST--
imap_open() DISABLE_AUTHENTICATOR ignores array param
--EXTENSIONS--
imap
--SKIPIF--
<?php
require_once(__DIR__. '/setup/imap_include.inc');

$in = @imap_open(IMAP_SERVER_DEBUG, IMAP_MAILBOX_USERNAME, IMAP_MAILBOX_PASSWORD, OP_HALFOPEN, 1);
if (!$in) {
    die("skip could not connect to mailbox " . IMAP_SERVER_DEBUG);
}
$kerberos = false;
if (is_array($errors = imap_errors())) {
    foreach ($errors as $err) {
        if (strstr($err, 'GSSAPI') || strstr($err, 'Kerberos')) {
            $kerberos = true;
        }
    }
}
if (!$kerberos) {
    die("skip need a GSSAPI/Kerberos aware server");
}
?>
--CONFLICTS--
defaultmailbox
--FILE--
<?php
// TODO Test Kerberos on CI
$tests = array(
    'Array'  => array('DISABLE_AUTHENTICATOR' => array('GSSAPI','NTLM')),
    'String' => array('DISABLE_AUTHENTICATOR' => 'GSSAPI'),
);
require_once(__DIR__. '/setup/imap_include.inc');
foreach ($tests as $name => $testparams) {
    echo "Test for $name\n";
    $in = imap_open(IMAP_SERVER_DEBUG, IMAP_MAILBOX_USERNAME, IMAP_MAILBOX_PASSWORD, OP_HALFOPEN, 1, $testparams);
    if ($in) {
        if (is_array($errors = imap_errors())) {
            foreach ($errors as $err) {
                if (strstr($err, 'GSSAPI') || strstr($err, 'Kerberos')) {
                    echo "$err\n";
                }
            }
        }
    } else {
        echo "Can't connect\n";
    }
}
echo "Done\n";
?>
--EXPECT--
Test for Array
Test for String
Done
