--TEST--
imagetype API with svg extension
--EXTENSIONS--
libxml
--FILE--
<?php

var_dump(IMAGETYPE_COUNT > IMAGETYPE_SVG);
var_dump(image_type_to_extension(IMAGETYPE_SVG));
var_dump(image_type_to_mime_type(IMAGETYPE_SVG));

?>
--EXPECT--
bool(true)
string(4) ".svg"
string(13) "image/svg+xml"
