--TEST--
Bug #72964 (White space not unfolded for CC/Bcc headers)
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only test');
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
require_once __DIR__ . '/mail_skipif.inc';
?>
--INI--
SMTP=localhost
smtp_port=25
--FILE--
<?php
require_once __DIR__ . '/mail_include.inc';

function find_and_delete_message($username, $subject) {
    global $default_mailbox, $password;

    $imap_stream = imap_open($default_mailbox, $username, $password);
    if ($imap_stream === false) {
        die("Cannot connect to IMAP server $server: " . imap_last_error() . "\n");
    }

    $found = false;
    $repeat_count = 20; // we will repeat a max of 20 times
    while (!$found && $repeat_count > 0) {
        // sleep for a while to allow msg to be delivered
        sleep(1);
    
        $num_messages = imap_check($imap_stream)->Nmsgs;
        for ($i = $num_messages; $i > 0; $i--) {
            $info = imap_headerinfo($imap_stream, $i);
            if ($info->subject === $subject) {
                $header = imap_fetchheader($imap_stream, $i);
                echo "X-Mailer header found: ";
                var_dump(strpos($header, 'X-Mailer: bug80706') !== false);
                imap_delete($imap_stream, $i);
                $found = true;
                break;
            }
        }
        $repeat_count--;
    }

    imap_close($imap_stream, CL_EXPUNGE);
    return $found;
}

$to = "{$users[1]}@$domain";
$subject = bin2hex(random_bytes(16));
$message = 'hello';
$headers = "From: webmaster@example.com\r\n"
    . "Bcc: {$users[2]}@$domain\r\n"
    . "X-Mailer: bug80706";

$res = mail($to, $subject, $message, $headers);
if ($res !== true) {
	die("TEST FAILED : Unable to send test email\n");
} else {
	echo "Message sent OK\n";
}

foreach ([$users[1], $users[2]] as $user) {
    if (!find_and_delete_message("$user@$domain", $subject)) {
        echo "TEST FAILED: email not delivered\n";
    } else {
        echo "TEST PASSED: Message sent and deleted OK\n";
    }
}
?>
--EXPECT--
Message sent OK
X-Mailer header found: bool(true)
TEST PASSED: Message sent and deleted OK
X-Mailer header found: bool(true)
TEST PASSED: Message sent and deleted OK
