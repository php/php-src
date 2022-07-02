--TEST--
Bug #80223 (imap_mail_compose() leaks envelope on malformed bodies)
--EXTENSIONS--
imap
--FILE--
<?php
try {
    imap_mail_compose([], []);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    imap_mail_compose([], [1]);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    imap_mail_compose([], [[]]);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
?>
--EXPECT--
imap_mail_compose(): Argument #2 ($bodies) cannot be empty
imap_mail_compose(): Argument #2 ($bodies) individual body must be of type array, int given
imap_mail_compose(): Argument #2 ($bodies) individual body cannot be empty
