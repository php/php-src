--TEST--
Bug #55701 (GlobIterator throws LogicException with message 'The parent constructor was not called')
--FILE--
<?php

//
// Some methods of GlobIterator do not throw a RuntimeException when the glob pattern doesn't match any file.
// Most methods of GlobIterator throw a RuntimeException when the glob pattern doesn't match any file
// because they get the properties of the current file
function testBaseClass($f) {
    // The tested iterator is in an invalid state; the behaviour of most of its methods is undefined
    try {
        $f();
        echo "ran normally (expected)\n";
    } catch (RuntimeException $e) {
        // Throwing a RuntimeException is the correct behaviour for some methods
        echo "ran normally (expected)\n";
    } catch (\Error $e) {
        // Throwing a LogicException is not correct
        echo "threw LogicException (unexpected)\n";
    }
}

//
// The derived classes must throw LogicException if the parent class constructor was not called
function testChildClass($f) {
    try {
        $f();
        echo "didn't throw (unexpected)\n";
    } catch (\Error $e) {
        echo "threw Error (expected)\n";
    } catch (Exception $e) {
        echo "threw other exception (unexpected)\n";
    }
}



//
// It must not throw LogicException when the iterator is not valid
echo "->count()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->count();
} );

echo "->rewind()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->rewind();
} );

echo "->getFlags()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->getFlags();
} );

echo "->setFlags()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->setFlags(FilesystemIterator::KEY_AS_PATHNAME);
} );

echo "->valid()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->valid();
} );



//
// When the iterator is not valid, the behaviour of the next methods is undefined
// Some of them throw a RuntimeException while others just return an invalid value
// However, they must not throw LogicException
echo "->current()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->current();
} );

echo "->key()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->key();
} );

echo "->next()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->next();
} );

echo "->getATime()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->getATime();
} );

echo "->getBasename()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->getBasename();
} );

echo "->getCTime()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->getCTime();
} );

echo "->getExtension()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->getExtension();
} );

echo "->getFilename()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->getFilename();
} );

echo "->getGroup()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->getGroup();
} );

echo "->getInode()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->getInode();
} );

echo "->getMTime()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->getMTime();
} );

echo "->getOwner()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->getOwner();
} );

echo "->getPath()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->getPath();
} );

echo "->getPathname()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->getPathname();
} );

echo "->getPerms()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->getPerms();
} );

echo "->getSize()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->getSize();
} );

echo "->getType()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->getType();
} );

echo "->isDir()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->isDir();
} );

echo "->isDot()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->isDot();
} );

echo "->isExecutable()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->isExecutable();
} );

echo "->isFile()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->isFile();
} );

echo "->isLink()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->isLink();
} );

echo "->isReadable()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->isReadable();
} );

echo "->isWritable()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->isWritable();
} );

echo "->seek()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->seek(0);
} );

echo "->__toString()... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.abcdefghij');
    $o->__toString();
} );


//
// Supplemental test: no method should throw LogicException if it is invoked
// after a successful iteration over a non-empty list of files.
echo "non-empty GlobIterator... ";
testBaseClass( function() {
    $o = new GlobIterator(__DIR__.'/*.phpt');
    foreach ($o as $file) {
        // nothing here, just consume all the items
    }
    // This must not throw an exception
    $o->count();
} );


//
// The correct existing behaviour must not change
// The classes SplFileObject and SplTempFileObject are not affected by the bug
echo "======================= test there are no regressions =======================\n";

echo "SplFileObject existent file... ";
testBaseClass( function() {
    $o = new SplFileObject(__FILE__);
    $o->fread(1);
} );

echo "SplFileObject non-existent file... ";
testBaseClass( function() {
    $o = new SplFileObject('/tmp/abcdefghij.abcdefghij');
    $o->fread(1);
} );



//
// Check that when derived classes do not call GlobIterator::__construct()
// the LogicException is thrown (don't break the behaviour introduced to fix bug #54384)
echo "extends GlobIterator... ";
class GlobIteratorChild extends GlobIterator {
    public function __construct() {}
}
testChildClass( function() {
    $o = new GlobIteratorChild();
    $o->count();
} );

echo "extends SplFileObject... ";
class SplFileObjectChild extends SplFileObject {
    public function __construct() {}
}
testChildClass( function() {
    $o = new SplFileObjectChild();
    $o->count();
} );

echo "extends SplTempFileObject... ";
class SplTempFileObjectChild extends SplTempFileObject {
    public function __construct() {}
}
testChildClass( function() {
    $o = new SplTempFileObjectChild();
    $o->count();
} );
?>
--EXPECT--
->count()... ran normally (expected)
->rewind()... ran normally (expected)
->getFlags()... ran normally (expected)
->setFlags()... ran normally (expected)
->valid()... ran normally (expected)
->current()... ran normally (expected)
->key()... ran normally (expected)
->next()... ran normally (expected)
->getATime()... ran normally (expected)
->getBasename()... ran normally (expected)
->getCTime()... ran normally (expected)
->getExtension()... ran normally (expected)
->getFilename()... ran normally (expected)
->getGroup()... ran normally (expected)
->getInode()... ran normally (expected)
->getMTime()... ran normally (expected)
->getOwner()... ran normally (expected)
->getPath()... ran normally (expected)
->getPathname()... ran normally (expected)
->getPerms()... ran normally (expected)
->getSize()... ran normally (expected)
->getType()... ran normally (expected)
->isDir()... ran normally (expected)
->isDot()... ran normally (expected)
->isExecutable()... ran normally (expected)
->isFile()... ran normally (expected)
->isLink()... ran normally (expected)
->isReadable()... ran normally (expected)
->isWritable()... ran normally (expected)
->seek()... ran normally (expected)
->__toString()... ran normally (expected)
non-empty GlobIterator... ran normally (expected)
======================= test there are no regressions =======================
SplFileObject existent file... ran normally (expected)
SplFileObject non-existent file... ran normally (expected)
extends GlobIterator... threw Error (expected)
extends SplFileObject... threw Error (expected)
extends SplTempFileObject... threw Error (expected)
