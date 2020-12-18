--TEST--
Bug #45705 test #2 (imap rfc822_parse_adrlist() modifies passed address parameter)
--SKIPIF--
<?php
extension_loaded('imap') or die('skip imap extension not available in this build');
?>
--FILE--
<?php

$envelope = array('return_path' => 'John Doe <john@example.com>',
                  'from'        => 'John Doe <john@example.com>',
                  'reply_to'    => 'John Doe <john@example.com>',
                  'to'          => 'John Doe <john@example.com>',
                  'cc'          => 'John Doe <john@example.com>',
                  'bcc'         => 'John Doe <john@example.com>',
);

var_dump($envelope);
imap_mail_compose($envelope, [1 => ['cc' => 'Steve Doe <steve@example.com>',]]);
var_dump($envelope);

?>
--EXPECT--
array(6) {
  ["return_path"]=>
  string(27) "John Doe <john@example.com>"
  ["from"]=>
  string(27) "John Doe <john@example.com>"
  ["reply_to"]=>
  string(27) "John Doe <john@example.com>"
  ["to"]=>
  string(27) "John Doe <john@example.com>"
  ["cc"]=>
  string(27) "John Doe <john@example.com>"
  ["bcc"]=>
  string(27) "John Doe <john@example.com>"
}
array(6) {
  ["return_path"]=>
  string(27) "John Doe <john@example.com>"
  ["from"]=>
  string(27) "John Doe <john@example.com>"
  ["reply_to"]=>
  string(27) "John Doe <john@example.com>"
  ["to"]=>
  string(27) "John Doe <john@example.com>"
  ["cc"]=>
  string(27) "John Doe <john@example.com>"
  ["bcc"]=>
  string(27) "John Doe <john@example.com>"
}
