--TEST--
Multipart Form POST Data, incorrect content length
--HEADERS--
return <<<END
Content-Type=multipart/form-data; boundary=---------------------------240723202011929
Content-Length=100
END;
--POST--
-----------------------------240723202011929
Content-Disposition: form-data; name="entry"

entry box
-----------------------------240723202011929
Content-Disposition: form-data; name="password"

password box
-----------------------------240723202011929
Content-Disposition: form-data; name="radio1"

test 1
-----------------------------240723202011929
Content-Disposition: form-data; name="checkbox1"

test 1
-----------------------------240723202011929
Content-Disposition: form-data; name="choices"

Choice 1
-----------------------------240723202011929
Content-Disposition: form-data; name="choices"

Choice 2
-----------------------------240723202011929
Content-Disposition: form-data; name="file"; filename="info.php"
Content-Type: application/octet-stream

<?php
phpinfo();
?>
-----------------------------240723202011929--
--FILE--
<?php
print @$_POST['choices'];
?>
--EXPECT--
