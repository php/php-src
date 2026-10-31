--TEST--
HWPX files with embedded PNG or Markdown names are not mistaken for OOXML
--EXTENSIONS--
fileinfo
--FILE--
<?php
$data = file_get_contents(__DIR__ . '/hwpx-with-preview.testfile');
$finfo = new finfo(FILEINFO_MIME_TYPE);
var_dump($finfo->file(__DIR__ . '/hwpx-with-preview.testfile'));
var_dump($finfo->buffer($data));
var_dump($finfo->buffer(str_replace('PrvImage.png', 'PrvImage.md_', $data)));

foreach ([
    'UEsDBBQAAAAIAEoOYV3ZwjYRFQAAABMAAAAIAAAAaWNvbi5wbmdLyywqLlFIy8xJVUjOzytJzSspBgBQSwMEFAAAAAgASg5hXZt9CDoMAAAACgAAAA4AAABleGFtcGxlLm51c3BlY7MpSEzOTkxP1bcDAFBLAwQUAAAACABKDmFdJRaDDg8AAAANAAAADAAAAGxpYi90ZXN0LmRsbCtJLS5RSM7PK0nNKykGAFBLAQIUAxQAAAAIAEoOYV3ZwjYRFQAAABMAAAAIAAAAAAAAAAAAAACAAQAAAABpY29uLnBuZ1BLAQIUAxQAAAAIAEoOYV2bfQg6DAAAAAoAAAAOAAAAAAAAAAAAAACAATsAAABleGFtcGxlLm51c3BlY1BLAQIUAxQAAAAIAEoOYV0lFoMODwAAAA0AAAAMAAAAAAAAAAAAAACAAXMAAABsaWIvdGVzdC5kbGxQSwUGAAAAAAMAAwCsAAAArAAAAAAA',
    'UEsDBBQAAAAIAEoOYV3ZwjYRFQAAABMAAAAJAAAAcmVhZG1lLm1kS8ssKi5RSMvMSVVIzs8rSc0rKQYAUEsDBBQAAAAIAEoOYV2bfQg6DAAAAAoAAAAOAAAAZXhhbXBsZS5udXNwZWOzKUhMzk5MT9W3AwBQSwMEFAAAAAgASg5hXSUWgw4PAAAADQAAAAwAAABsaWIvdGVzdC5kbGwrSS0uUUjOzytJzSspBgBQSwECFAMUAAAACABKDmFd2cI2ERUAAAATAAAACQAAAAAAAAAAAAAAgAEAAAAAcmVhZG1lLm1kUEsBAhQDFAAAAAgASg5hXZt9CDoMAAAACgAAAA4AAAAAAAAAAAAAAIABPAAAAGV4YW1wbGUubnVzcGVjUEsBAhQDFAAAAAgASg5hXSUWgw4PAAAADQAAAAwAAAAAAAAAAAAAAIABdAAAAGxpYi90ZXN0LmRsbFBLBQYAAAAAAwADAK0AAACtAAAAAAA=',
] as $encoded) {
    var_dump($finfo->buffer(base64_decode($encoded)));
}
?>
--EXPECT--
string(21) "application/x-hwp+zip"
string(21) "application/x-hwp+zip"
string(21) "application/x-hwp+zip"
string(29) "application/vnd.nuget.package"
string(29) "application/vnd.nuget.package"
