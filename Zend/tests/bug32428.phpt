--TEST--
Bug #32428 (The @ warning error suppression operator is broken)
--FILE--
<?php
  $data = @$not_exists;
  $data = @($not_exists);
  $data = @!$not_exists;
  $data = !@$not_exists;
  $data = @($not_exists+1);
  echo "ok\n";
?>
--EXPECT--
ok
