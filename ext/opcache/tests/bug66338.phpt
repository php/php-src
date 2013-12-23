--TEST--
Bug #66338 (Optimization binding of class constants is not safely opcacheable)
--INI--
opcache.enable=0
--SKIPIF--
<?php if (!extension_loaded('Zend OPcache') || php_sapi_name() != "cli") die("skip CLI only"); ?>
--FILE--
<?php
$root  = str_replace('.php', "", __FILE__);
$base  = basename( $root );

file_put_contents( "$root-Officials.inc", '<?php 
	class Officials { static function getLeader() { return LocalTerms::GOV_LEADER; } }
	' );

file_put_contents( "$root-clientUS.php", '<?php 
	class LocalTerms { const GOV_LEADER = "Barack Hussein Obama II"; }
	require "'.$root.'-Officials.inc";
	printf( "The President of the USA is %s\n", Officials::getLeader() );
	' );

file_put_contents( "$root-clientUK.php", '<?php 
	class LocalTerms { const GOV_LEADER = "David William Donald Cameron"; }
	require "'.$root.'-Officials.inc";
	printf( "The Prime Minister of the UK is %s\n", Officials::getLeader() );
	' );

include "php_cli_server.inc";
$uri = sprintf("http://%s/%s", PHP_CLI_SERVER_ADDRESS, basename(__FILE__));
$opt = -1;   # This test works if $opt = 0
php_cli_server_start("-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.optimization_level=$opt -d opcache.file_update_protection=0" );

echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/$base-clientUS.php" );
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/$base-clientUK.php" );

unlink("$root-Officials.inc");
unlink("$root-clientUS.php");
unlink("$root-clientUK.php");
?>
--EXPECT--
The President of the USA is Barack Hussein Obama II
The Prime Minister of the UK is David William Donald Cameron
