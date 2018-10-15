--TEST--
mb_output_handler() (EUC-JP)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
// TODO: Do real test

// EUC-JP
$euc_jp = "テスト用日本語文字列。このモジュールはPHPにマルチバイト関数を提供します。";
mb_http_output('EUC-JP') or print("mb_http_output() failed\n");
ob_start('mb_output_handler');
echo $euc_jp;
$output = ob_get_clean();

var_dump( $output );

?>
--EXPECT--
string(73) "テスト用日本語文字列。このモジュールはPHPにマルチバイト関数を提供します。"
