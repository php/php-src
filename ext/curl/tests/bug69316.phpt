--TEST--
Bug #69316: Use-after-free in php_curl related to CURLOPT_FILE/_INFILE/_WRITEHEADER
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
  function hdr_callback($ch, $data) {
      // close the stream, causing the FILE structure to be free()'d
      if($GLOBALS['f_file']) {
          fclose($GLOBALS['f_file']); $GLOBALS['f_file'] = 0;

          // cause an allocation of approx the same size as a FILE structure, size varies a bit depending on platform/libc
          $FILE_size = (PHP_INT_SIZE == 4 ? 0x160 : 0x238);
          curl_setopt($ch, CURLOPT_COOKIE, str_repeat("a", $FILE_size - 1));
      }
      return strlen($data);
  }

  include 'server.inc';
  $host = curl_cli_server_start();
  $temp_file = __DIR__ . '/body.tmp';
  $url = "{$host}/get.inc?test=getpost";
  $ch = curl_init();
  $f_file = fopen($temp_file, "w") or die("failed to open file\n");
  curl_setopt($ch, CURLOPT_BUFFERSIZE, 10);
  curl_setopt($ch, CURLOPT_HEADERFUNCTION, "hdr_callback");
  curl_setopt($ch, CURLOPT_FILE, $f_file);
  curl_setopt($ch, CURLOPT_URL, $url);
  curl_exec($ch);
  curl_close($ch);
?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . '/body.tmp');
?>
--EXPECTF--
Warning: curl_exec(): CURLOPT_FILE resource has gone away, resetting to default in %s on line %d
array(1) {
  ["test"]=>
  string(7) "getpost"
}
array(0) {
}
===DONE===
