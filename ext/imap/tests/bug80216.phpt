--TEST--
Bug #80216 (imap_mail_compose() does not validate types/encodings)
--EXTENSIONS--
imap
--FILE--
<?php
imap_mail_compose([], [['type' => TYPEMULTIPART], []]);
imap_mail_compose([], [['type' => 12]]);
imap_mail_compose([], [['type' => TYPEMULTIPART], ['type' => 12]]);
imap_mail_compose([], [['encoding' => 8]]);
imap_mail_compose([], [['type' => TYPEMULTIPART], ['encoding' => 8]]);
echo "done\n";
?>
--EXPECT--
done
