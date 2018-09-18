--TEST--
imap_open() DISABLE_AUTHENTICATOR ignores array param
--SKIPIF--
<?php
extension_loaded('imap') or die('skip imap extension not available in this build');

require_once(dirname(__FILE__).'/imap_include.inc');

$in = imap_open($default_mailbox, $username, $password, OP_HALFOPEN, 1);
if (!$in) {
    die("skip could not connect to mailbox $default_mailbox");
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
--FILE--
<?php
$tests = array(
    'Array'  => array('DISABLE_AUTHENTICATOR' => array('GSSAPI','NTLM')),
    'String' => array('DISABLE_AUTHENTICATOR' => 'GSSAPI'),
);
require_once(dirname(__FILE__).'/imap_include.inc');
foreach ($tests as $name => $testparams) {
    echo "Test for $name\n";
    $in = imap_open($default_mailbox, $username, $password, OP_HALFOPEN, 1, $testparams);
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
