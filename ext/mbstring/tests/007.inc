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
