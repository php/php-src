--TEST--
GH-17039 (PHP 8.4: Incorrect MIME content type)
--EXTENSIONS--
fileinfo
--FILE--
<?php

class Foo
{
    public function bar()
    {
        $this->baz(function () {});
    }
}

echo mime_content_type(__FILE__);

?>
--EXPECT--
text/x-php
