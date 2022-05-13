--TEST--
Bug #80215 (imap_mail_compose() may modify by-val parameters)
--EXTENSIONS--
imap
--FILE--
<?php
$envelope = [
    "from" => 1,
    "to" => 2,
    "custom_headers" => [3],
];
$body = [[
    "contents.data" => 4,
    "type.parameters" => ['foo' => 5],
    "disposition" => ['bar' => 6],
], [
    "contents.data" => 7,
    "type.parameters" => ['foo' => 8],
    "disposition" => ['bar' => 9],
]];
imap_mail_compose($envelope, $body);
var_dump($envelope, $body);
?>
--EXPECT--
array(3) {
  ["from"]=>
  int(1)
  ["to"]=>
  int(2)
  ["custom_headers"]=>
  array(1) {
    [0]=>
    int(3)
  }
}
array(2) {
  [0]=>
  array(3) {
    ["contents.data"]=>
    int(4)
    ["type.parameters"]=>
    array(1) {
      ["foo"]=>
      int(5)
    }
    ["disposition"]=>
    array(1) {
      ["bar"]=>
      int(6)
    }
  }
  [1]=>
  array(3) {
    ["contents.data"]=>
    int(7)
    ["type.parameters"]=>
    array(1) {
      ["foo"]=>
      int(8)
    }
    ["disposition"]=>
    array(1) {
      ["bar"]=>
      int(9)
    }
  }
}
