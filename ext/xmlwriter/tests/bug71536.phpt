--TEST--
Bug #71536 (Access Violation crashes php-cgi.exe)
--EXTENSIONS--
xmlwriter
--FILE--
<?php
class Test {
    public static function init()
    {
        $xml = new \XMLWriter();
        $xml->openUri('php://memory');
        $xml->setIndent(false);
        $xml->startDocument('1.0', 'UTF-8');
        $xml->startElement('response');
        die('now'); // crashed with die()
    }
}

Test::init();
?>
--EXPECT--
now
