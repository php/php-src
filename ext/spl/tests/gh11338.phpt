--TEST--
GH-11338 (SplFileInfo empty getBasename with more than on slash)
--FILE--
<?php

function test($path) {
    echo "Testing: '$path'\n";
    $file = new \SplFileInfo($path);
    var_dump($file->getBasename());
    var_dump($file->getFilename());
}

test('/dir/anotherdir/basedir//');
test('/dir/anotherdir/basedir/');
test('/dir/anotherdir/basedir');
test('/dir/anotherdir//basedir');
test('///');
test('//');
test('/');
test('');

?>
--EXPECT--
Testing: '/dir/anotherdir/basedir//'
string(7) "basedir"
string(7) "basedir"
Testing: '/dir/anotherdir/basedir/'
string(7) "basedir"
string(7) "basedir"
Testing: '/dir/anotherdir/basedir'
string(7) "basedir"
string(7) "basedir"
Testing: '/dir/anotherdir//basedir'
string(7) "basedir"
string(7) "basedir"
Testing: '///'
string(0) ""
string(1) "/"
Testing: '//'
string(0) ""
string(1) "/"
Testing: '/'
string(0) ""
string(1) "/"
Testing: ''
string(0) ""
string(0) ""
