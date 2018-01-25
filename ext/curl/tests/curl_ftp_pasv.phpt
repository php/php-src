--TEST--
Test curl_exec() function with basic functionality
--CREDITS--
--SKIPIF--
<?php
if (!extension_loaded("curl")) exit("skip curl extension not loaded");
if (false === getenv('PHP_CURL_FTP_REMOTE_SERVER'))  exit("skip PHP_CURL_FTP_REMOTE_SERVER env variable is not defined");
if (false === getenv('PHP_CURL_FTP_REMOTE_USER'))  exit("skip PHP_CURL_FTP_REMOTE_USER env variable is not defined");
if (false === getenv('PHP_CURL_FTP_REMOTE_PASSWD'))  exit("skip PHP_CURL_FTP_REMOTE_PASSWD env variable is not defined");
?>
--FILE--
<?php
  $host = getenv('PHP_CURL_FTP_REMOTE_SERVER');
  $username = getenv('PHP_CURL_FTP_REMOTE_USER');
  $password = getenv('PHP_CURL_FTP_REMOTE_PASSWD');

  // FTP this script to a server
  $fp  =  fopen ( __FILE__ ,  "r" );
  $url  =  "ftp://$username:$password@$host/test.phpt" ;

  $ch  =  curl_init ();

  // enable below to get the output in verbose mode.
  // curl_setopt ( $ch , CURLOPT_VERBOSE, 1 );

  /* Without enabling SKIP_PASV_IP flag, the following output will be seen..
	< 227 Entering Passive Mode (10,5,80,146,100,199)
	*   Trying 10.5.80.146... * connected
	* Connecting to 10.5.80.146 (10.5.80.146) port 25799
   */

  /* After enabling SKIP_PASV_IP flag, the following output will be seen..
	< 227 Entering Passive Mode (10,5,80,146,50,229)
	* Skips 10.5.80.146 for data connection, uses 10.5.80.146 instead
	*   Trying 10.5.80.146... * connected
   */

  curl_setopt ( $ch , CURLOPT_URL, $url );
  curl_setopt ( $ch , CURLOPT_TRANSFERTEXT, 1 );

  //force passive connection
  curl_setopt ( $ch , CURLOPT_FTP_USE_EPSV, 0 );
  curl_setopt ( $ch , CURLOPT_FTP_SKIP_PASV_IP, 1 );

  // mark the file for upload..
  curl_setopt ( $ch , CURLOPT_INFILE, $fp );
  curl_setopt ( $ch , CURLOPT_INFILESIZE,  filesize(__FILE__) );
  curl_setopt ( $ch , CURLOPT_PUT, 1 );
  curl_setopt ( $ch , CURLOPT_UPLOAD, 1 );

  $result  =  curl_exec ( $ch );
  var_dump ( $result );
  curl_close ( $ch );

?>
===DONE===
--EXPECTF--
bool(true)
===DONE===
