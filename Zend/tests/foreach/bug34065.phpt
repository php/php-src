--TEST--
Bug #34065 (throw in foreach causes memory leaks)
--FILE--
<?php
$data = file(__FILE__);
try {
  foreach ($data as $line) {
    throw new Exception("error");
  }
} catch (Exception $e) {
  echo "ok\n";
}
?>
--EXPECT--
ok
