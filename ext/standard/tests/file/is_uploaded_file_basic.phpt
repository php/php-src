--TEST--
is_uploaded_file() function
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php if (php_sapi_name()=='cli') die('skip'); ?>
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

// Error cases
var_dump(is_uploaded_file());
var_dump(is_uploaded_file('a', 'b'));

?>
--EXPECTF--
bool(true)
bool(false)
bool(false)
bool(false)

Warning: Wrong parameter count for is_uploaded_file() in %s on line %d
NULL

Warning: Wrong parameter count for is_uploaded_file() in %s on line %d
NULL

