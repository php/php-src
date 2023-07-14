--TEST--
GH-10990 (mail() throws TypeError after iterating over $additional_headers array by reference)
--INI--
sendmail_path=rubbish 2>/dev/null
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Won't run on Windows");
?>
--FILE--
<?php
$from = 'test@example.com';
$headers = ['From' => &$from];
var_dump(mail('test@example.com', 'Test', 'Test', $headers));
?>
--EXPECT--
bool(false)
