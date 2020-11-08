--TEST--
Test getimagesize() function : variation - Passing non image files
--FILE--
<?php
$file_types_array = array (
    //File containing text string
    "File with text data" => "test.txt",

    //File containing forcibly corrupted bmp image
    "File with corrupted BMP data" => "200x100_unknown.unknown",

    //File which doesn't exist
     "Non-existent file" => "nofile.ext",

    //File having no data
    "Empty File" => "blank_file.bmp"
);

echo "*** Testing getimagesize() : variation ***\n";

//loop through each element of the array for filename
foreach($file_types_array as $key => $filename) {
      echo "\n-- $key ($filename) --\n";
      var_dump( getimagesize(__DIR__."/$filename" ) );
      var_dump( getimagesize(__DIR__."/$filename", $info) );
      var_dump( $info );
};
?>
--EXPECTF--
*** Testing getimagesize() : variation ***

-- File with text data (test.txt) --
bool(false)
bool(false)
array(0) {
}

-- File with corrupted BMP data (200x100_unknown.unknown) --
bool(false)
bool(false)
array(0) {
}

-- Non-existent file (nofile.ext) --

Warning: getimagesize(%snofile.ext): Failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: getimagesize(%snofile.ext): Failed to open stream: No such file or directory in %s on line %d
bool(false)
array(0) {
}

-- Empty File (blank_file.bmp) --

Notice: getimagesize(): Error reading from %s! in %s on line %d
bool(false)

Notice: getimagesize(): Error reading from %s! in %s on line %d
bool(false)
array(0) {
}
