--TEST--
Bug #28228 (number_format() does not allow empty decimal separator)
--FILE--
<?php
echo number_format(1234.5678, 4, '', '') . "\n";
echo number_format(1234.5678, 4, NULL, ',') . "\n";
echo number_format(1234.5678, 4, 0, ',') . "\n";
echo number_format(1234.5678, 4);
?>
--EXPECT--
12345678
1,234.5678
1,23405678
1,234.5678
