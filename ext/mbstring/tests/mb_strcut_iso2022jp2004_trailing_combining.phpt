--TEST--
mb_strcut() keeps trailing combining base character in ISO-2022-JP-2004 after JIS X 0213 plane 1
--EXTENSIONS--
mbstring
--FILE--
<?php
$s = mb_convert_encoding("\u{4FF1}か", 'ISO-2022-JP-2004', 'UTF-8');
echo bin2hex($s), "\n";
echo bin2hex(mb_strcut($s, 0, strlen($s), 'ISO-2022-JP-2004')), "\n";
?>
--EXPECT--
1b2428512e21242b1b2842
1b2428512e21242b1b2842
