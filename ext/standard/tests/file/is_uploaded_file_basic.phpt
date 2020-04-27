--TEST--
is_uploaded_file() function
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--POST_RAW--
Content-type: multipart/form-data, boundary=AaB03x

--AaB03x
content-disposition: form-data; name="field1"

Joe Blow
--AaB03x
content-disposition: form-data; name="pics"; filename="file1.txt"
Content-Type: text/plain

abcdef123456789
--AaB03x--
--FILE--
<?php
// uploaded file
var_dump(is_uploaded_file($_FILES['pics']['tmp_name']));

// not an uploaded file
var_dump(is_uploaded_file($_FILES['pics']['name']));

// not an uploaded file
var_dump(is_uploaded_file('random_filename.txt'));

// not an uploaded file
var_dump(is_uploaded_file('__FILE__'));

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
