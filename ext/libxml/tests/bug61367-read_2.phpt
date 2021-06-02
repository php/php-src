--TEST--
Bug #61367: open_basedir bypass in libxml RSHUTDOWN: read test
--SKIPIF--
<?php
if(!extension_loaded('dom')) echo 'skip dom extension not available';
if (LIBXML_VERSION < 20912) die('skip For libxml2 >= 2.9.12 only');
?>
--INI--
open_basedir=.
--FILE--
<?php
/*
 * Note: Using error_reporting=E_ALL & ~E_NOTICE to suppress "Trying to get property of non-object" notices.
 */
class StreamExploiter {
    public function stream_close (  ) {
        $doc = new DOMDocument;
        $doc->resolveExternals = true;
        $doc->substituteEntities = true;
        $dir = htmlspecialchars(dirname(getcwd()));
        $dir = str_replace('\\', '/', $dir); // fix for windows
        $doc->loadXML( <<<XML
<!DOCTYPE doc [
    <!ENTITY file SYSTEM "file:///$dir/bad">
]>
<doc>&file;</doc>
XML
        );
        print $doc->documentElement->firstChild->nodeValue;
    }

    public function stream_open (  $path ,  $mode ,  $options ,  &$opened_path ) {
        return true;
    }
}

var_dump(mkdir('test_bug_61367-read'));
var_dump(mkdir('test_bug_61367-read/base'));
var_dump(file_put_contents('test_bug_61367-read/bad', 'blah'));
var_dump(chdir('test_bug_61367-read/base'));

stream_wrapper_register( 'exploit', 'StreamExploiter' );
$s = fopen( 'exploit://', 'r' );

?>
--CLEAN--
<?php
unlink('test_bug_61367-read/bad');
rmdir('test_bug_61367-read/base');
rmdir('test_bug_61367-read');
?>
--EXPECTF--
bool(true)
bool(true)
int(4)
bool(true)

Warning: DOMDocument::loadXML(): I/O warning : failed to load external entity "file:///%s/test_bug_61367-read/bad" in %s on line %d

Warning: Attempt to read property "nodeValue" on null in %s on line %d
