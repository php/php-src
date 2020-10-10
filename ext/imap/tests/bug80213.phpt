--TEST--
Bug #80213 (imap_mail_compose() segfaults on certain $bodies)
--SKIPIF--
<?php
if (!extension_loaded('imap')) die('skip imap extension not available');
?>
--FILE--
<?php
$envelope = [];
$body = [[
    'type.parameters' => ['param'],
    'disposition' => ['disp'],
], [
    'type.parameters' => ['param'],
    'disposition' => ['disp'],
]];
imap_mail_compose($envelope, $body);
echo "done\n";
?>
--EXPECT--
done
