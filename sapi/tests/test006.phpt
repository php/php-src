--TEST--
Multipart Form POST Data
--HEADERS--
return <<<END
Content-Type=multipart/form-data; boundary=---------------------------240723202011929
Content-Length=862
END;
--ENV--
return <<<END
CONTENT_TYPE=multipart/form-data; boundary=---------------------------240723202011929
CONTENT_LENGTH=862
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
error_reporting(0);
print_r($_POST);
print_r($_FILES);
?>
--EXPECTF--
Array
(
    [entry] => entry box
    [password] => password box
    [radio1] => test 1
    [checkbox1] => test 1
    [choices] => Choice 2
)
Array
(
    [file] => Array
        (
            [name] => info.php
            [type] => application/octet-stream
            [tmp_name] => %s
            [error] => 0
            [size] => 21
        )

)
