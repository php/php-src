--TEST--
Test trampoline for curl option CURLOPT_READFUNCTION
--EXTENSIONS--
curl
--FILE--
<?php

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
	    return 0;
    }
}
$o = new TrampolineTest();
$callback = [$o, 'trampoline'];

$sFileBase  = __DIR__.DIRECTORY_SEPARATOR.'curl_read_trampoline';
$sReadFile  = $sFileBase.'_in.tmp';
$sWriteFile = $sFileBase.'_out.tmp';
$sWriteUrl  = 'file://'.$sWriteFile;

file_put_contents($sReadFile,'contents of tempfile');
$hReadHandle = fopen($sReadFile, 'r');

$oCurl = curl_init();
curl_setopt($oCurl, CURLOPT_URL,          $sWriteUrl);
curl_setopt($oCurl, CURLOPT_UPLOAD,       1);
curl_setopt($oCurl, CURLOPT_READFUNCTION, $callback);
curl_setopt($oCurl, CURLOPT_INFILE,       $hReadHandle );
curl_exec($oCurl);
curl_close($oCurl);

fclose ($hReadHandle);

$sOutput = file_get_contents($sWriteFile);
var_dump($sOutput);

?>
--CLEAN--
<?php
$sFileBase  = __DIR__.DIRECTORY_SEPARATOR.'curl_read_trampoline';
$sReadFile  = $sFileBase.'_in.tmp';
$sWriteFile = $sFileBase.'_out.tmp';
@unlink($sReadFile);
@unlink($sWriteFile);
?>
--EXPECT--
Trampoline for trampoline
string(0) ""
