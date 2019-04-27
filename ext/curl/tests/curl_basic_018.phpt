--TEST--
Test curl_setopt() with curl_multi function with basic functionality
--CREDITS--
TestFest 2009 - AFUP - Thomas Rabaix <thomas.rabaix@gmail.com>
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
/* Prototype  : bool curl_setopt(resource ch, int option, mixed value)
 * Description: Set an option for a cURL transfer
 * Source code: ext/curl/interface.c
 * Alias to functions:
 */

  include 'server.inc';
  $host = curl_cli_server_start();

  // start testing
  echo "*** Testing curl_exec() : basic functionality ***\n";

  $url = "{$host}/get.inc?test=get";
  $chs = array(
    0 => curl_init(),
    1 => curl_init(),
    2 => curl_init(),
  );

  ob_start(); // start output buffering

  $options = array(
    CURLOPT_RETURNTRANSFER => 1,
    CURLOPT_URL => $url,
  );

  curl_setopt_array($chs[0], $options); //set the options
  curl_setopt_array($chs[1], $options); //set the options
  curl_setopt_array($chs[2], $options); //set the options

  $mh = curl_multi_init();

  // add handlers
  curl_multi_add_handle($mh, $chs[0]);
  curl_multi_add_handle($mh, $chs[1]);
  curl_multi_add_handle($mh, $chs[2]);

  $running=null;
  //execute the handles
  do {
    curl_multi_exec($mh, $running);
  } while ($running > 0);

  $curl_content = '';
  $curl_content .= curl_multi_getcontent($chs[0]);
  $curl_content .= curl_multi_getcontent($chs[1]);
  $curl_content .= curl_multi_getcontent($chs[2]);

  //close the handles
  curl_multi_remove_handle($mh, $chs[0]);
  curl_multi_remove_handle($mh, $chs[1]);
  curl_multi_remove_handle($mh, $chs[2]);
  curl_multi_close($mh);

  var_dump( $curl_content );

?>
===DONE===
--EXPECT--
*** Testing curl_exec() : basic functionality ***
string(75) "Hello World!
Hello World!Hello World!
Hello World!Hello World!
Hello World!"
===DONE===
