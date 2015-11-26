--TEST--
cURL option CURLOPT_READFUNCTION
--CREDITS--
WHITE new media architects - Jeroen Vermeulen
#testfest Utrecht 2009
--SKIPIF--
<?php 
if (!extension_loaded("curl")) print "skip cURL extension not loaded"; 
?>
--FILE--
<?php    
function custom_readfunction($oCurl, $hReadHandle, $iMaxOut) 
{
  $sData = fread($hReadHandle,$iMaxOut-10); # -10 to have space to add "custom:"
  if (!empty($sData))
  { 
    $sData = "custom:".$sData;
  }
  return $sData;
}

$sFileBase  = dirname(__FILE__).DIRECTORY_SEPARATOR.'curl_opt_CURLOPT_READFUNCTION';
$sReadFile  = $sFileBase.'_in.tmp';
$sWriteFile = $sFileBase.'_out.tmp';
$sWriteUrl  = 'file://'.$sWriteFile;

file_put_contents($sReadFile,'contents of tempfile');
$hReadHandle = fopen($sReadFile, 'r');

$oCurl = curl_init();
curl_setopt($oCurl, CURLOPT_URL,          $sWriteUrl);
curl_setopt($oCurl, CURLOPT_UPLOAD,       1);
curl_setopt($oCurl, CURLOPT_READFUNCTION, "custom_readfunction" );
curl_setopt($oCurl, CURLOPT_INFILE,       $hReadHandle );
curl_exec($oCurl);
curl_close($oCurl);

fclose ($hReadHandle); 

$sOutput = file_get_contents($sWriteFile); 
var_dump($sOutput);
?>
===DONE===
--CLEAN--
<?php
$sFileBase  = dirname(__FILE__).DIRECTORY_SEPARATOR.'curl_opt_CURLOPT_READFUNCTION';
$sReadFile  = $sFileBase.'_in.tmp';
$sWriteFile = $sFileBase.'_out.tmp';
unlink($sReadFile);
unlink($sWriteFile);
?>
--EXPECT--
string(27) "custom:contents of tempfile"
===DONE===
