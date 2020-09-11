--TEST--
Bug #54384: Several SPL classes crash when the parent constructor is not called
--FILE--
<?php

function test($f) {
    try {
        $f();
        echo "ran normally (unexpected)\n\n";
    } catch (\Error $e) {
        echo "Error (expected)\n";
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
?>
--EXPECT--
IteratorIterator... Error (expected)
FilterIterator... Error (expected)
RecursiveFilterIterator... Error (expected)
ParentIterator... Error (expected)
LimitIterator... Error (expected)
CachingIterator... Error (expected)
RecursiveCachingIterator... Error (expected)
NoRewindIterator... Error (expected)
RegexIterator... Error (expected)
RecursiveRegexIterator... Error (expected)
GlobIterator... Error (expected)
SplFileObject... Error (expected)
SplTempFileObject... Error (expected)
AppendIterator... Error (expected)
InfiniteIterator... Error (expected)
