--TEST--
Test [key: $value] alias for ['key' => $value]
--FILE--
<?php

// In every array syntax where "'key' => " can be used, "key: " can be used instead.
$values = [key: 'other'];
var_export($values);
echo "\n";

[key: $key] = $values;
echo "key=$key\n";

?>
--EXPECT--
array (
  'key' => 'other',
)
key=other
