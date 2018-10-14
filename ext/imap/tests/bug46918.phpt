--TEST--
Bug #46918 (imap_rfc822_parse_adrlist host part not filled in correctly)
--SKIPIF--
<?php
        if (!extension_loaded("imap")) {
                die("skip imap extension not available");
        }
?>
--FILE--
<?php

$adds = 'ian eiloart <iane@example.ac.uk>,
      shuf6@example.ac.uk,
      blobby,
      "ian,eiloart"<ian@example.ac.uk>,
      <@example.com:foo@example.ac.uk>,
      foo@#,
      ian@-example.com,
      ian@one@two';
$add_arr = imap_rfc822_parse_adrlist($adds, 'example.com');
var_export($add_arr);

?>
--EXPECT--
array (
  0 => 
  stdClass::__set_state(array(
     'mailbox' => 'iane',
     'host' => 'example.ac.uk',
     'personal' => 'ian eiloart',
  )),
  1 => 
  stdClass::__set_state(array(
     'mailbox' => 'shuf6',
     'host' => 'example.ac.uk',
  )),
  2 => 
  stdClass::__set_state(array(
     'mailbox' => 'blobby',
     'host' => 'example.com',
  )),
  3 => 
  stdClass::__set_state(array(
     'mailbox' => 'ian',
     'host' => 'example.ac.uk',
     'personal' => 'ian,eiloart',
  )),
  4 => 
  stdClass::__set_state(array(
     'mailbox' => 'foo',
     'host' => 'example.ac.uk',
     'adl' => '@example.com',
  )),
  5 => 
  stdClass::__set_state(array(
     'mailbox' => 'foo',
     'host' => '#',
  )),
  6 => 
  stdClass::__set_state(array(
     'mailbox' => 'ian',
     'host' => '-example.com',
  )),
  7 => 
  stdClass::__set_state(array(
     'mailbox' => 'ian',
     'host' => 'one',
  )),
  8 => 
  stdClass::__set_state(array(
     'mailbox' => 'UNEXPECTED_DATA_AFTER_ADDRESS',
     'host' => '.SYNTAX-ERROR.',
  )),
)
Notice: Unknown: Unexpected characters at end of address: @two (errflg=3) in Unknown on line 0
