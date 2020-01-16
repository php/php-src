--TEST--
iptcembed() valid jpg stream
--FILE--
<?php
/*
# source code to generate base64 use behind as $base64_1x1_jpeg
# we don't want to be gd library dependant for this test
$file="1x1.jpg";
$ret=imagejpeg(imagecreatetruecolor(1, 1), $file, 100);
echo md5(file_get_contents($file)).PHP_EOL;
echo base64_encode(file_get_contents($file)).PHP_EOL;
unlink($file);
*/

/*
test description :
1) create local file 1x1 jpeg (without iptc) (use base64 content to create file)
2) generate iptcdata string with function iptc_make_tag describe behind
3) use iptcembed php function with our 1x1 jpeg file and our iptcdata string
4) write local file2 with iptcembed return content
5) various check on file2 to verify that's a valid jpeg file with our tags
*/


#iptc_make_tag function from http://php.net/iptcembed
function iptc_make_tag($rec, $data, $value)
  {
    $length = strlen($value);
    $retval = chr(0x1C) . chr($rec) . chr($data);
    if($length < 0x8000) { $retval .= chr($length >> 8) .  chr($length & 0xFF); }
    else { $retval .= chr(0x80) .  chr(0x04) .  chr(($length >> 24) & 0xFF) .  chr(($length >> 16) & 0xFF) .  chr(($length >> 8) & 0xFF) .  chr($length & 0xFF); }
    return $retval . $value;
  }


$file="1x1.jpg";
$file2="1x1_with_iptc_tags.jpg";
$base64_1x1_jpeg="/9j/4AAQSkZJRgABAQEAYABgAAD//gA8Q1JFQVRPUjogZ2QtanBlZyB2MS4wICh1c2luZyBJSkcgSlBFRyB2ODApLCBxdWFsaXR5ID0gMTAwCv/bAEMAAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAf/bAEMBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAf/AABEIAAEAAQMBEQACEQEDEQH/xAAfAAABBQEBAQEBAQAAAAAAAAAAAQIDBAUGBwgJCgv/xAC1EAACAQMDAgQDBQUEBAAAAX0BAgMABBEFEiExQQYTUWEHInEUMoGRoQgjQrHBFVLR8CQzYnKCCQoWFxgZGiUmJygpKjQ1Njc4OTpDREVGR0hJSlNUVVZXWFlaY2RlZmdoaWpzdHV2d3h5eoOEhYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4eLj5OXm5+jp6vHy8/T19vf4+fr/xAAfAQADAQEBAQEBAQEBAAAAAAAAAQIDBAUGBwgJCgv/xAC1EQACAQIEBAMEBwUEBAABAncAAQIDEQQFITEGEkFRB2FxEyIygQgUQpGhscEJIzNS8BVictEKFiQ04SXxFxgZGiYnKCkqNTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqCg4SFhoeIiYqSk5SVlpeYmZqio6Slpqeoqaqys7S1tre4ubrCw8TFxsfIycrS09TV1tfY2dri4+Tl5ufo6ery8/T19vf4+fr/2gAMAwEAAhEDEQA/AP8AP/oA/9k=";
#write file
$fd=fopen($file,"wb");
if ($fd) { fputs($fd,base64_decode($base64_1x1_jpeg)); fclose($fd); }
else { echo "error can't write $file".PHP_EOL;exit(1); }
#check file md5
$md5=md5_file($file);
if ($md5!="07dd8594450e8c18ab8a79d7cb4573c7") { echo "md5 error".PHP_EOL;exit(1); }
#check jpeg properties
list($width, $height, $type, $attr) = getimagesize($file,$info);
if ($width!=1) { echo "width error".PHP_EOL;exit(1); }
if ($height!=1) { echo "height error".PHP_EOL;exit(1); }
if ($type!=2) { echo "type error".PHP_EOL;exit(1); }
if (!isset($info["APP0"])) { echo "APP0 error".PHP_EOL;exit(1); }

#our iptc tags
$tags=array();
$tags["2#105"]= "Tauren";
$tags["2#120"]= "Tauren with Trunk";
$tags["2#110"]= "Copyright 2004-2016, Blizzard";
$tags["2#025"]= "Tauren, Chaman, Blizzard";
$tags["2#090"]= "Thunder Bluffs";
#feed iptc string for iptcembed
$iptc='';
foreach ($tags as $tag => $string) { $rec=$tag[0]; $tag = substr($tag, 2); $iptc .= iptc_make_tag($rec, $tag, $string); }
#check iptc string md5
if (md5(base64_encode($iptc))!="7056c4b3060f92a4f9e5b7d0caa61859") { echo "iptc md5 error".PHP_EOL;exit(1); }

#use iptcembed to get jpeg stream content with iptc tags
$content = iptcembed($iptc, $file,0);

#write new image with iptc tags
if ($content === false) {echo "iptcembed error".PHP_EOL;exit(1); }
$fd=fopen($file2,"wb");
if ($fd) { fputs($fd,$content); fclose($fd); }
else { echo "error can't write $file2".PHP_EOL;exit(1); }


#check jpeg properties for new image with iptc tags
echo "new generated image with itpc tags : $file2".PHP_EOL;
$ret = getimagesize($file2,$info);
if ($ret===false) { echo "getimagesize error".PHP_EOL;exit(1); }
list($width, $height, $type, $attr) = $ret;
if ($width!=1) { echo "width error".PHP_EOL;exit(1); }
if ($height!=1) { echo "height error".PHP_EOL;exit(1); }
if ($type!=2) { echo "type error".PHP_EOL;exit(1); }
if (!isset($info["APP0"])) { echo "APP0 error".PHP_EOL;exit(1); }
if (!isset($info["APP13"])) { echo "APP13 error".PHP_EOL;exit(1); }

$error=0;
$iptc_data_from_created_image = iptcparse($info['APP13']);
foreach ($tags as $tag => $string) {
  #check if tag exists
  if (!isset($iptc_data_from_created_image[$tag])) {
    echo "error iptc tag $tag not found".PHP_EOL;
    $error++;
  } else {
    #check value
    if ($iptc_data_from_created_image[$tag][0]!=$string) {
      echo "error tag $tag : bad value ($string != ".$iptc_data_from_created_image[$tag][0].")".PHP_EOL;
      $error++;
    }
  }
}
#clean before exit
@unlink($file);
@unlink($file2);
if ($error==0) { echo "OK".PHP_EOL;exit(0);}
echo "something wrong: $error errors".PHP_EOL;
?>
--EXPECT--
new generated image with itpc tags : 1x1_with_iptc_tags.jpg
OK
