<?php
// Change these to make tests run successfully
$server   = '{localhost}';
$default_mailbox = $server . "INBOX";
$domain = "example.com";
$admin_user = "webmaster"; // a user with admin access
$username = "$admin_user@$domain";
$password = 'p4ssw0rd';
$users = array("webmaster", "info", "admin", "foo"); // tests require 4 valid userids
$mailbox_prefix = "phpttest"; // name used for test mailbox

/**
 * Create a test mailbox and populate with msgs
 *
 * @para, string mailbox_suffix Suffix used to uniquely identify mailboxes
 * @param int message_count number of test msgs to be written to new mailbox
 *
 * @return IMAP stream to new mailbox on success; FALSE on failure
 */
function setup_test_mailbox($mailbox_suffix, $message_count, &$new_mailbox = null, $msg_type = "simple"){
    global $server, $default_mailbox, $username, $password;

    // open a stream to default mailbox
    $imap_stream = imap_open($default_mailbox, $username, $password);

    if ($imap_stream === false) {
        echo "Cannot connect to IMAP server $server: " . imap_last_error() . "\n";
        return false;
    }

    echo "Create a temporary mailbox and add " . $message_count .  " msgs\n";
    $new_mailbox = create_mailbox($imap_stream, $mailbox_suffix, $message_count, $msg_type);
    if ($new_mailbox === false) {
       echo "Can't create a temporary mailbox: " . imap_last_error(). "\n";
       return false;
    }

    echo ".. mailbox '$new_mailbox' created\n";

    // reopen stream to new mailbox
    if (imap_reopen($imap_stream, $new_mailbox) === false) {
        echo "can't re-open '$new_mailbox' mailbox: " . imap_last_error() . "\n";
        return false;
    }

    return $imap_stream;
}

/**
 * Create mailbox and fill with generic emails
 *
 * @param resource $imap_stream
 * @param string $mailbox
 */
function create_mailbox($imap_stream, $mailbox_suffix, $message_count, $msg_type= "simple"){
    global $default_mailbox, $mailbox_prefix;
    $mailbox = $default_mailbox . "." . $mailbox_prefix . $mailbox_suffix;

    $mailboxes = imap_getmailboxes($imap_stream, $mailbox, '*');

    // check mailbox does not already exist
    if ($mailboxes) {
        foreach($mailboxes as $value) {
            if ($value->name == $mailbox) {
                exit ("TEST FAILED : Mailbox '$mailbox' already exists\n");
            }
        }
    }

    if (imap_createmailbox($imap_stream, $mailbox) === false) {
        return false;
    }

    // Add number of test msgs requested
    if ($message_count > 0) {
        populate_mailbox($imap_stream, $mailbox, $message_count, $msg_type);
    }

    return $mailbox;
}

/**
 * Populate a mailbox with generic emails
 *
 * @param resource $imap_stream
 * @param string $mailbox
 */
function populate_mailbox($imap_stream, $mailbox, $message_count, $msg_type = "simple"){

    global $users, $domain;

    for($i = 1; $i <= $message_count; $i++) {
        if ($msg_type == "simple") {
            $msg =  "From: foo@anywhere.com\r\n"
                . "To: $users[0]@$domain\r\n"
                . "Subject: test$i\r\n"
                . "\r\n"
                . "$i: this is a test message, please ignore\r\n";
        } else {
            $envelope["from"]= "foo@anywhere.com";
            $envelope["to"]  = "$users[0]@$domain";
            $envelope["subject"] = "Test msg $i";

            $part1["type"] = TYPEMULTIPART;
            $part1["subtype"] = "mixed";

            $part2["type"] = TYPETEXT;
            $part2["subtype"] = "plain";
            $part2["description"] = "imap_mail_compose() function";
            $part2["contents.data"] = "message 1:xxxxxxxxxxxxxxxxxxxxxxxxxx";

            $part3["type"] = TYPETEXT;
            $part3["subtype"] = "plain";
            $part3["description"] = "Example";
            $part3["contents.data"] = "message 2:yyyyyyyyyyyyyyyyyyyyyyyyyy";

            $part4["type"] = TYPETEXT;
            $part4["subtype"] = "plain";
            $part4["description"] = "Return Values";
            $part4["contents.data"] = "message 3:zzzzzzzzzzzzzzzzzzzzzzzzzz";

            $body[1] = $part1;
            $body[2] = $part2;
            $body[3] = $part3;
            $body[4] = $part4;

            $msg = imap_mail_compose($envelope, $body);
        }

        imap_append($imap_stream, $mailbox, $msg);
    }
}

/**
 * Get the mailbox name from a mailbox description, i.e strip off server details.
 *
 * @param string mailbox complete mailbox name
 * @return mailbox name
 */
function get_mailbox_name($mailbox){

    if (preg_match('/\{.*?\}(.*)/', $mailbox, $match) != 1) {
        echo "Unrecpognized mailbox name\n";
        return false;
    }

    return $match[1];
}

?>
