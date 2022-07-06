--TEST--
Test mail() function : basic functionality
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != 'WIN' ) {
   die('skip...Windows only test');
}

require_once(__DIR__.'/mail_skipif.inc');
?>
--INI--
max_execution_time = 120
--FILE--
<?php
echo "*** Testing mail() : basic functionality ***\n";
require_once(__DIR__.'/mail_include.inc');
$subject_prefix = "!**PHPT**!";

$to = "$username";
$subject = "$subject_prefix: Basic PHPT test for mail() function";
$message = <<<HERE
Description
bool mail ( string \$to , string \$subject , string \$message [, string \$additional_headers [, string \$additional_parameters]] )
Send an email message
HERE;

$extra_headers = "from: user@example.com";

$res = mail($to, $subject, $message, $extra_headers);

if ($res !== true) {
    exit("TEST FAILED : Unable to send test email\n");
} else {
    echo "Msg sent OK\n";
}

// Search for email message on the mail server using imap.
$imap_stream = imap_open($default_mailbox, $username, $password);
if ($imap_stream === false) {
    echo "Cannot connect to IMAP server $server: " . imap_last_error() . "\n";
    return false;
}

$found = false;
$repeat_count = 20; // we will repeat a max of 20 times
while (!$found && $repeat_count > 0) {

    // sleep for a while to allow msg to be delivered
    sleep(1);

    $current_msg_count = imap_check($imap_stream)->Nmsgs;

    // Iterate over recent msgs to find the one we sent above
    for ($i = 1; $i <= $current_msg_count; $i++) {
        // get hdr details
        $hdr = imap_headerinfo($imap_stream, $i);

        if (substr($hdr->Subject, 0 , strlen($subject_prefix)) == $subject_prefix) {
            echo "Id of msg just sent is $i\n";
            echo ".. delete it\n";
            imap_delete($imap_stream, $i);
            $found = true;
            break;
        }
    }

    $repeat_count -= 1;
}

if (!$found) {
    echo "TEST FAILED: email not delivered\n";
} else {
    echo "TEST PASSED: Msgs sent and deleted OK\n";
}

imap_close($imap_stream, CL_EXPUNGE);
?>
--EXPECTF--
*** Testing mail() : basic functionality ***
Msg sent OK
Id of msg just sent is %d
.. delete it
TEST PASSED: Msgs sent and deleted OK
