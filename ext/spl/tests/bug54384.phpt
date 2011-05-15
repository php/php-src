--TEST--
Bug #54384: Several SPL classes crash when the parent constructor is not called
--FILE--
<?php

function test($f) {
	try {
		$f();
		echo "ran normally (unexpected)\n\n";
	} catch (LogicException $e) {
		echo "exception (expected)\n";
	}
}

echo "IteratorIterator... ";
class IteratorIteratorTest extends IteratorIterator {
    function __construct(){}
}
test( function() {
	$o = new IteratorIteratorTest;
	$o->rewind();
} );

echo "FilterIterator... ";
class FilterIteratorTest extends FilterIterator {
    function __construct(){}
    function accept(){}
}
test( function() {
	$o = new FilterIteratorTest;
	$o->rewind();
} );

echo "RecursiveFilterIterator... ";
class RecursiveFilterIteratorTest extends RecursiveFilterIterator {
    function __construct(){}
    function accept(){}
}
test( function() {
$o = new RecursiveFilterIteratorTest;
$o->hasChildren();
} );

echo "ParentIterator... ";
class ParentIteratorTest extends ParentIterator {
    function __construct(){}
}
test ( function() {
$o = new ParentIteratorTest;
$o->accept();
} );

echo "LimitIterator... ";
class LimitIteratorTest extends LimitIterator {
    function __construct(){}
}
test ( function() {
$o = new LimitIteratorTest;
$o->rewind();
} );

echo "CachingIterator... ";
class CachingIteratorTest extends CachingIterator {
    function __construct(){}
}
test ( function() {
$o = new CachingIteratorTest;
$o->rewind();
} );

echo "RecursiveCachingIterator... ";
class RecursiveCachingIteratorTest extends RecursiveCachingIterator {
    function __construct(){}
}
test ( function() {
$o = new RecursiveCachingIteratorTest;
$o->rewind();
} );

echo "NoRewindIterator... ";
class NoRewindIteratorTest extends NoRewindIterator {
    function __construct(){}
}
test ( function() {
$o = new NoRewindIteratorTest;
$o->valid();
} );

echo "RegexIterator... ";
class RegexIteratorTest extends RegexIterator {
    function __construct(){}
}
test ( function() {
$o = new RegexIteratorTest;
$o->rewind();
} );

echo "RecursiveRegexIterator... ";
class RecursiveRegexIteratorTest extends RecursiveRegexIterator {
    function __construct(){}
}
test ( function() {
$o = new RecursiveRegexIteratorTest;
$o->hasChildren();
} );

echo "GlobIterator... ";
class GlobIteratorTest extends GlobIterator {
    function __construct(){}
}
test ( function() {
$o = new GlobIteratorTest;
$o->count();
} );

echo "SplFileObject... ";
class SplFileObjectTest extends SplFileObject {
    function __construct(){}
}
test ( function() {
$o = new SplFileObjectTest;
$o->rewind();
} );

echo "SplTempFileObject... ";
class SplTempFileObjectTest extends SplTempFileObject {
    function __construct(){}
}
test ( function() {
$o = new SplTempFileObjectTest;
$o->rewind();
} );


echo "SplFileInfo... ";
class SplFileInfoTest extends SplFileInfo {
    function __construct(){}
}
test ( function() {
$o = new SplFileInfoTest;
$o->getMTime();
} );

echo "DirectoryIterator... ";
class DirectoryIteratortest extends DirectoryIterator {
    function __construct(){}
}
test ( function() {
$o = new DirectoryIteratorTest;
foreach ($o as $a) {
echo $a,"\n";
}
} );

echo "DirectoryIterator (exception cleared)... ";
class DirectoryIteratorTest2 extends DirectoryIterator {
    function __construct(){
		try {
			parent::__construct();
		} catch (Exception $e) { }
	}
}
test ( function() {
$o = new DirectoryIteratorTest2;
foreach ($o as $a) {
echo $a,"\n";
}
} );

echo "FileSystemIterator... ";
class FileSystemIteratorTest extends DirectoryIterator {
    function __construct(){}
}
test ( function() {
$o = new FileSystemIteratorTest;
foreach ($o as $a) {
echo $a,"\n";
}
} );

echo "RecursiveDirectoryIterator... ";
class RecursiveDirectoryIteratorTest extends RecursiveDirectoryIterator {
    function __construct(){}
}
test ( function() {
$o = new RecursiveDirectoryIteratorTest;
foreach ($o as $a) {
echo $a,"\n";
}
} );

echo "RecursiveTreeIterator... ";
class RecursiveTreeIteratorTest extends RecursiveDirectoryIterator {
    function __construct(){}
}
test ( function() {
$o = new RecursiveTreeIteratorTest;
foreach ($o as $a) {
echo $a,"\n";
}
} );

echo "AppendIterator... ";
class AppendIteratorTest extends AppendIterator {
    function __construct(){}
}
test ( function() {
$o = new AppendIteratorTest;
foreach ($o as $a) {
echo $a,"\n";
}
} );

echo "InfiniteIterator... ";
class InfiniteIteratorTest extends InfiniteIterator {
    function __construct(){}
}
test ( function() {
$o = new InfiniteIteratorTest;
foreach ($o as $a) {
echo $a,"\n";
}
} );

echo "CallbackFilterIterator... ";
class CallbackFilterIteratorTest extends CallbackFilterIterator {
    function __construct(){}
}
test ( function() {
$o = new CallbackFilterIteratorTest;
foreach ($o as $a) {
echo $a,"\n";
}
} );

echo "RecursiveCallbackFilterIterator... ";
class RecursiveCallbackFilterIteratorTest extends RecursiveCallbackFilterIterator {
    function __construct(){}
}
test ( function() {
$o = new RecursiveCallbackFilterIteratorTest;
foreach ($o as $a) {
echo $a,"\n";
}
} );


--EXPECT--
IteratorIterator... exception (expected)
FilterIterator... exception (expected)
RecursiveFilterIterator... exception (expected)
ParentIterator... exception (expected)
LimitIterator... exception (expected)
CachingIterator... exception (expected)
RecursiveCachingIterator... exception (expected)
NoRewindIterator... exception (expected)
RegexIterator... exception (expected)
RecursiveRegexIterator... exception (expected)
GlobIterator... exception (expected)
SplFileObject... exception (expected)
SplTempFileObject... exception (expected)
SplFileInfo... exception (expected)
DirectoryIterator... exception (expected)
DirectoryIterator (exception cleared)... exception (expected)
FileSystemIterator... exception (expected)
RecursiveDirectoryIterator... exception (expected)
RecursiveTreeIterator... exception (expected)
AppendIterator... exception (expected)
InfiniteIterator... exception (expected)
CallbackFilterIterator... exception (expected)
RecursiveCallbackFilterIterator... exception (expected)
