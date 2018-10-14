--TEST--
Bug #53377 (imap_mime_header_decode() doesn't ignore \t during long MIME header unfolding)
--SKIPIF--
<?php
        if (!extension_loaded("imap")) {
                die("skip imap extension not available");
        }
?>
--FILE--
<?php
$s = "=?UTF-8?Q?=E2=82=AC?=";
$header = "$s\n $s\n\t$s";

var_dump(imap_mime_header_decode($header));
--EXPECT--
array(3) {
  [0]=>
  object(stdClass)#1 (2) {
    ["charset"]=>
    string(5) "UTF-8"
    ["text"]=>
    string(3) "€"
  }
  [1]=>
  object(stdClass)#2 (2) {
    ["charset"]=>
    string(5) "UTF-8"
    ["text"]=>
    string(3) "€"
  }
  [2]=>
  object(stdClass)#3 (2) {
    ["charset"]=>
    string(5) "UTF-8"
    ["text"]=>
    string(3) "€"
  }
}
