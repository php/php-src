<?php
extension_loaded('imap') or die('skip imap extension not available in this build');

if( substr(PHP_OS, 0, 3) == 'WIN' && extension_loaded('sockets')) {
    // be sure mail server is accessible... on PHP 5.3.13 release build, using test-pack PHP-5.3-r1af8b3f,
    // the code below didn't skip test even though there was no mail server
    //     test then failed (no mail server to test against)
    $socket  = socket_create(AF_INET, SOCK_RAW, 1);
    socket_set_option($socket, SOL_SOCKET, SO_RCVTIMEO, array('sec' => 10, 'usec' => 10));
    // imap uses tcp port 143
    socket_connect($socket, "localhost", 143) or die ("skip can't socket to mail server");
}

// Change these to make tests run successfully
$mailbox  = '{localhost}';
$username = 'webmaster@example.com';
$password = 'p4ssw0rd';
$options = OP_HALFOPEN; // this should be enough to verify server present
$retries = 0; // don't retry connect on failure

$mbox = @imap_open($mailbox, $username, $password, $options, $retries);
if (!$mbox) {
    die("skip could not connect to mailbox $mailbox");
}
imap_close($mbox);
?>
