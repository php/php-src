--TEST--
Test curl_multi_exec() function with basic functionality
--CREDITS--
TestFest 2009 - AFUP - Thomas Rabaix <thomas.rabaix@gmail.com>
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
/* Prototype  : bool curl_multi_exec(resource ch)
 * Description: Perform a cURL session
 * Source code: ext/curl/multi.c
 * Alias to functions:
 */

  include 'server.inc';
  $host = curl_cli_server_start();

  // start testing
  echo "*** Testing curl_exec() : basic functionality ***\n";

  $url = "{$host}/get.php?test=get";
  $chs = array(
    0 => curl_init(),
    1 => curl_init(),
    2 => curl_init(),
  );

  ob_start(); // start output buffering

  curl_setopt($chs[0], CURLOPT_URL, $url); //set the url we want to use
  curl_setopt($chs[1], CURLOPT_URL, $url); //set the url we want to use
  curl_setopt($chs[2], CURLOPT_URL, $url); //set the url we want to use

  $mh = curl_multi_init();

  // add handlers
  curl_multi_add_handle($mh, $chs[0]);
  curl_multi_add_handle($mh, $chs[1]);
  curl_multi_add_handle($mh, $chs[2]);

  $running=null;
  //execute the handles
  $state = null;
  do {
    $state = curl_multi_exec($mh, $running);
  } while ($running > 0);

  //close the handles
  curl_multi_remove_handle($mh, $chs[0]);
  curl_multi_remove_handle($mh, $chs[1]);
  curl_multi_remove_handle($mh, $chs[2]);
  curl_multi_close($mh);

  $curl_content = ob_get_contents();
  ob_end_clean();

  if($state === CURLM_OK) {
    var_dump( $curl_content );
  } else {
    echo "curl_exec returned false";
  }
?>
===DONE===
--EXPECT--
*** Testing curl_exec() : basic functionality ***
string(75) "Hello World!
Hello World!Hello World!
Hello World!Hello World!
Hello World!"
===DONE===
