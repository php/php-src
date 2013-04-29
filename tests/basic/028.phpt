--TEST--
RFC1867 character quotting
--INI--
file_uploads=1
max_input_vars=1000
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name=name1

testname
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name='name2'

testname
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="name3"

testname
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name=name\4

testname
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name=name\\5

testname
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name=name\'6

testname
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name=name\"7

testname
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name='name\8'

testname
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name='name\\9'

testname
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name='name\'10'

testname
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name='name\"11'

testname
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="name\12"

testname
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="name\\13"

testname
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="name\'14"

testname
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="name\"15"

testname
-----------------------------20896060251896012921717172737--
--FILE--
<?php
var_dump($_POST);
?>
--EXPECT--
array(15) {
  ["name1"]=>
  string(8) "testname"
  ["name2"]=>
  string(8) "testname"
  ["name3"]=>
  string(8) "testname"
  ["name\4"]=>
  string(8) "testname"
  ["name\5"]=>
  string(8) "testname"
  ["name\'6"]=>
  string(8) "testname"
  ["name\"7"]=>
  string(8) "testname"
  ["name\8"]=>
  string(8) "testname"
  ["name\9"]=>
  string(8) "testname"
  ["name'10"]=>
  string(8) "testname"
  ["name\"11"]=>
  string(8) "testname"
  ["name\12"]=>
  string(8) "testname"
  ["name\13"]=>
  string(8) "testname"
  ["name\'14"]=>
  string(8) "testname"
  ["name"15"]=>
  string(8) "testname"
}
