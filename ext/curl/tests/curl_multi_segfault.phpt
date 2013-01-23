--TEST--
Segfault due to libcurl connection caching 
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
  $url  =  "ftp://$username:$password@$host/" ;

  $ch  =  curl_init ();

  curl_setopt ( $ch , CURLOPT_URL, $url );
  curl_setopt ( $ch , CURLOPT_RETURNTRANSFER, 1 );

  //force passive connection
  curl_setopt ( $ch , CURLOPT_FTP_USE_EPSV, 0 );
  curl_setopt ( $ch , CURLOPT_FTP_SKIP_PASV_IP, 1 );

  $cmh =  curl_multi_init();
  curl_multi_add_handle($cmh, $ch);

  $active = null;

  do {
	  $mrc = curl_multi_exec($cmh, $active);
  } while ($mrc == CURLM_CALL_MULTI_PERFORM);


  while ($active && $mrc == CURLM_OK) {
	  if (curl_multi_select($cmh) != -1) {
		  do {
			  $mrc = curl_multi_exec($cmh, $active);
		  } while ($mrc == CURLM_CALL_MULTI_PERFORM);
	  }
  }   
    
  var_dump(is_string(curl_multi_getcontent($ch)));
  curl_multi_remove_handle($cmh, $ch);
  curl_close($ch);
  curl_multi_close($cmh);
?>
===DONE===
--EXPECTF--
bool(true)
===DONE===
