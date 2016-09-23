--TEST--
Bug #73037 SoapServer reports Bad Request when gzipped, var 0
--SKIPIF--
<?php 
	die("skip test needs improvement");
	require_once('skipif.inc');
	if (!extension_loaded("zlib")) {
		echo "skip zlib is required for this test";
	}
?>
--FILE--
<?php

function get_data($max)
{
	$piece = "<CD>
		<TITLE>Empire Burlesque</TITLE>
		<ARTIST>Bob Dylan</ARTIST>
		<COUNTRY>USA</COUNTRY>
		<COMPANY>Columbia</COMPANY>
		<PRICE>10.90</PRICE>
		<YEAR>1985</YEAR>
	</CD>";

	$begin = '<?xml version="1.0" encoding="UTF-8"?><soapenv:Envelope xmlns:soapenv="http://www.w3.org/2003/05/soap-envelope" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"><soapenv:Body><CATALOG>';
	$end = '</CATALOG></soapenv:Body></soapenv:Envelope>';

	$min = strlen($begin) + strlen($piece) + strlen($end);
	$max = $max < $min ? $min : $max;

	$data = $begin;
	$data .= $piece;
	while (strlen($data) + strlen($end) < $max) {
		/* Randomize a bit, taking gzip in account. */
		$tmp = str_replace(
			array(
				"Empire Burlesque",
				"Bob Dylan",
			),
			array(
				md5(uniqid()),
				sha1(uniqid()),
			),
			$piece
		);

		if (strlen($begin) + strlen($tmp) + strlen($end) > $max) {
			break;
		}

		$data .= $tmp;
	}
	$data .= $end;

	return $data;
}

$code = <<<'PHP'
$s = new SoapServer(NULL, array('uri' => 'http://here'));
$s->setObject(new stdclass());
$s->handle();
PHP;

$router = "bug73037_server.php";
file_put_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . $router, '<?php ' . $code . ' ?>');
	
$host = "localhost";
$port = "8965";
$desc = array(
	0 => STDIN,
	1 => STDOUT,
	2 => STDERR,
);
/*$desc = array(
	array("pipe", "r"),
	array("pipe", "w"),
	array("pipe", "w")
);*/
if (substr(PHP_OS, 0, 3) == 'WIN') {
	$cmd = getenv('TEST_PHP_EXECUTABLE') . " -t " . dirname(__FILE__) .  " -S $host:$port";
	$handle = proc_open($cmd, $desc, $pipes, dirname(__FILE__), NULL, array("bypass_shell" => true,  "suppress_errors" => true));
} else {
	$cmd = getenv('TEST_PHP_EXECUTABLE') . " -t " . dirname(__FILE__) .  " -S $host:$port 2>/dev/null";
	$handle = proc_open($cmd, $desc, $pipes, dirname(__FILE__));
}
//echo stream_get_contents($pipes[1]), "\n", stream_get_contents($pipes[2]);

foreach (array(1024-1, 1024*8-3, 1024*9+1, 1024*16-1, 1024*32-5, 1024*64+3, 1024*128-7) as $k => $i) {
	echo "Iteration $k\n\n";

	/* with and without compression */
	foreach (array(false, true) as $b) {
		$data = get_data($i);
		if ($b) {
			$data = gzencode($data);
		}
		$len = strlen($data);

		//echo "len=$len\n";

		$hdrs = <<<HDRS
POST /bug73037_server.php HTTP/1.1
Content-Type: application/soap+xml; charset=UTF-8
Accept: application/soap+xml, application/dime, multipart/related, text/*
SOAPAction: "urn:adressen#adressen#SetAda"
Expect: 100-continue
Content-Length: ${len}
HDRS;
		if ($b) {
			$hdrs .="\nContent-Encoding: gzip";		
		}
		//echo "Headers sent:\n$hdrs\n\n";
		$fp = fsockopen($host, $port, $errno, $errstr, 5);
		if (!$fp) {
		  die("connect failed");
		}

		if(fwrite($fp, "$hdrs\n\n$data")) {
			$out = "";
			while (!feof($fp)) {
				$out .= fread($fp, 1024);
			}

			$pos = strpos($out, "<env:Text>");
			if (false === $pos) {
				echo $out;
				goto cleanup;
			}
			$pos0 = $pos + strlen("<env:Text>");
			$pos = strpos($out, "</env:Text>");
			if (false === $pos) {
				echo $out;
				goto cleanup;
			}
			$len = $pos - $pos0;
			echo substr($out, $pos0, $len);
		}

cleanup:
		fclose($fp);

		echo "\n\n";
		}
}

proc_terminate($handle);

?>
==DONE==
--CLEAN--
<?php
unlink(dirname(__FILE__) . DIRECTORY_SEPARATOR . "bug73037_server.php");
?>
--EXPECT--
Iteration 0

Function 'CATALOG' doesn't exist

Function 'CATALOG' doesn't exist

Iteration 1

Function 'CATALOG' doesn't exist

Function 'CATALOG' doesn't exist

Iteration 2

Function 'CATALOG' doesn't exist

Function 'CATALOG' doesn't exist

Iteration 3

Function 'CATALOG' doesn't exist

Function 'CATALOG' doesn't exist

Iteration 4

Function 'CATALOG' doesn't exist

Function 'CATALOG' doesn't exist

Iteration 5

Function 'CATALOG' doesn't exist

Function 'CATALOG' doesn't exist

Iteration 6

Function 'CATALOG' doesn't exist

Function 'CATALOG' doesn't exist

==DONE==
