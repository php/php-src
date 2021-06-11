--TEST--
Bug #80242 (imap_mail_compose() segfaults for multipart with rfc822)
--EXTENSIONS--
imap
--FILE--
<?php
$bodies = [[
    'type' => TYPEMULTIPART,
], [
    'type' => TYPETEXT,
    'contents.data' => 'some text',
], [
    'type' => TYPEMESSAGE,
    'subtype' => 'RFC822',
]];
imap_mail_compose([], $bodies);
echo "done\n";
?>
--EXPECT--
done
