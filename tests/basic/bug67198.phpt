--TEST--
php://input is empty when enable_post_data_reading=Off
--INI--
allow_url_fopen=1
--CONFLICTS--
server
--SKIPIF--
<?php
include __DIR__."/../../sapi/cli/tests/skipif.inc";
?>
--FILE--
<?php
require __DIR__."/../../sapi/cli/tests/php_cli_server.inc";

$code =
<<<'FL'
 if(!ini_get('enable_post_data_reading')){
  if($_SERVER['REQUEST_METHOD']=='POST'){
   exit(file_get_contents('php://input'));
  }
 }else{
  exit('Please SET php.ini: enable_post_data_reading = Off');
 }
FL;

$postdata = "PASS";

$opts = array('http' =>
    array(
        'method'  => 'POST',
        'header'  => 'Content-type: application/x-www-form-urlencoded',
        'content' => $postdata
    )
);

$context  = stream_context_create($opts);

php_cli_server_start(
    "exit(file_get_contents('php://input'));", null,
    ["-d", "enable_post_data_reading=Off"]);

var_dump(file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS, false, $context));
var_dump(file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS, false, $context));
--EXPECT--
string(4) "PASS"
string(4) "PASS"
