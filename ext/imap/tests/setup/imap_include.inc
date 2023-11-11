<?php
/** If required change these values to make the test runs */
const IMAP_SERVER_NO_DEBUG = '{127.0.0.1:143/norsh}';
const IMAP_SERVER_DEBUG = '{127.0.0.1:143/debug/norsh}';
const IMAP_SERVER = IMAP_SERVER_DEBUG;
const IMAP_DEFAULT_MAILBOX = IMAP_SERVER . 'INBOX';
const IMAP_MAIL_DOMAIN = 'example.com';
const IMAP_ADMIN_USER = 'webmaster'; // a user with admin access
const IMAP_MAILBOX_USERNAME = IMAP_ADMIN_USER . '@' . IMAP_MAIL_DOMAIN;
const IMAP_MAILBOX_PASSWORD = 'p4ssw0rd';
const IMAP_MAILBOX_PHPT_PREFIX = 'phpttest';
/** Tests require 4 valid userids */
const IMAP_USERS = ["webmaster", "info", "admin", "foo"];

/** list of fields to expect */
const MANDATORY_OVERVIEW_FIELDS = [
    'size',
    'uid',
    'msgno',
    'recent',
    'flagged',
    'answered',
    'deleted',
    'seen',
    'draft',
    'udate',
];

// record test start time (used by displayOverviewFields())
$start_time = time();

/**
 * Display all fields in an element from an imap_fetch_overview() response
 *
 * Special handling for 'udate', which will vary run-to-run; assumes an IMAP
 * server with its clock synced to the current system, which is consistent with
 * setup instructions in ext/imap/tests/README.md
 *
 * @param $resp
 * @param string[] $fields
 */
function displayOverviewFields($resp, array $fields = MANDATORY_OVERVIEW_FIELDS) {
    global $start_time;
    foreach ($fields as $mf) {
        $z = $resp->$mf;
        if ($mf == 'udate') {
            if (($z >= $start_time) && ($z <= time())) {
                echo "$mf is OK\n";
            } else {
                echo "$mf is BAD ($z)\n";
            }
        } else {
            echo "$mf is $z\n";
        }
    }
}


/**
 * Create a test mailbox and populate with msgs
 *
 * @param string mailbox_suffix Suffix used to uniquely identify mailboxes
 * @param int message_count number of test msgs to be written to new mailbox
 * @param null $new_mailbox
 * @param bool $simpleMessages
 * @param int $flags OP_* (or CL_EXPUNGE) flags to pass to imap_open() sub-call
 * @return resource IMAP stream to new mailbox
 * @throws Exception
 */
function setup_test_mailbox(
    string $mailbox_suffix,
    int $message_count,
    &$new_mailbox = null,
    bool $simpleMessages = true,
    int $flags = 0,
){
    // open a stream to default mailbox
    $imap_stream = imap_open(IMAP_DEFAULT_MAILBOX, IMAP_MAILBOX_USERNAME, IMAP_MAILBOX_PASSWORD, flags: $flags);

    if ($imap_stream === false) {
        throw new Exception("Cannot connect to IMAP server " . IMAP_SERVER . ": " . imap_last_error());
    }

    echo "Create a temporary mailbox and add " . $message_count .  " msgs\n";
    $new_mailbox = create_mailbox($imap_stream, $mailbox_suffix, $message_count, $simpleMessages);

    echo "New mailbox created\n";

    // reopen stream to new mailbox
    if (imap_reopen($imap_stream, $new_mailbox) === false) {
        throw new Exception("Can't re-open '$new_mailbox' mailbox: " . imap_last_error());
    }

    return $imap_stream;
}

/**
 * Create mailbox and fill with generic emails
 *
 * @param resource $imap_stream
 * @param string $mailbox_suffix
 * @param int $message_count
 * @param bool $simpleMessages
 * @return string
 * @throws Exception
 */
function create_mailbox($imap_stream, string $mailbox_suffix, int $message_count, bool $simpleMessages = true): string {
    $mailbox = IMAP_DEFAULT_MAILBOX . '.' . IMAP_MAILBOX_PHPT_PREFIX . $mailbox_suffix;

    $mailboxes = imap_getmailboxes($imap_stream, $mailbox, '*');

    // check mailbox does not already exist
    if ($mailboxes) {
        foreach($mailboxes as $value) {
            if ($value->name == $mailbox) {
                throw new Exception("Mailbox '$mailbox' already exists");
            }
        }
    }

    if (imap_createmailbox($imap_stream, $mailbox) === false) {
        throw new Exception("Can't create a temporary mailbox: " . imap_last_error());
    }

    // Add number of test msgs requested
    if ($message_count > 0) {
        populate_mailbox($imap_stream, $mailbox, $message_count, $simpleMessages);
    }

    return $mailbox;
}

function setup_test_mailbox_for_uid_tests(string $mailbox_suffix, &$msg_no = null, &$msg_uid = null)
{
    $mail_box = setup_test_mailbox($mailbox_suffix, 10);
    echo "Delete 4 messages for Unique ID generation\n";
    // Delete messages to remove the numerical ordering
    imap_delete($mail_box, 3);
    imap_delete($mail_box, 4);
    imap_delete($mail_box, 5);
    imap_delete($mail_box, 6);
    imap_expunge($mail_box);
    $msg_no = 5;
    $msg_uid = 9;

    return $mail_box;
}

/**
 * Populate a mailbox with generic emails
 *
 * @param resource $imap_stream
 * @param string $mailbox
 * @param int $message_count
 * @param bool $simpleMessages
 */
function populate_mailbox($imap_stream, string $mailbox, int $message_count, bool $simpleMessages = true): void {
    for ($i = 1; $i <= $message_count; $i++) {
        if ($simpleMessages) {
            $msg =  "From: foo@anywhere.com\r\n"
                . "To: ". IMAP_USERS[0] . "@" . IMAP_MAIL_DOMAIN . "\r\n"
                . "Subject: test$i\r\n"
                . "\r\n"
                . "$i: this is a test message, please ignore\r\nnewline";
        } else {
            $envelope["from"]= "foo@anywhere.com";
            $envelope["to"]  = IMAP_USERS[0] . "@" . IMAP_MAIL_DOMAIN;
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
 * @return string mailbox name
 */
function get_mailbox_name(string $mailboxName): string {

    if (preg_match('/\{.*?\}(.*)/', $mailboxName, $match) != 1) {
        throw new Exception("Unrecognized mailbox name '$mailboxName'");
    }

    return $match[1];
}
