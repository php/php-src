--TEST--
Simple multi-byte print test (EUC-JP)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
--FILE--
<?php
/*
 * Test basic PHP functions to check if it works with multi-byte chars
 */

// EUC-JP strings
$s1 = "マルチバイト関数が使えます。";
$s2 = "この文字が連結されているはず。";

// print directly
echo "echo: ".$s1.$s2."\n";
print("print: ".$s1.$s2."\n");
printf("printf: %s%s\n",$s1, $s2);
echo sprintf("sprintf: %s%s\n",$s1, $s2);

// Assign to var
$s3 = $s1.$s2."\n";
echo "echo: ".$s3;

?>
--EXPECT--
echo: マルチバイト関数が使えます。この文字が連結されているはず。
print: マルチバイト関数が使えます。この文字が連結されているはず。
printf: マルチバイト関数が使えます。この文字が連結されているはず。
sprintf: マルチバイト関数が使えます。この文字が連結されているはず。
echo: マルチバイト関数が使えます。この文字が連結されているはず。
