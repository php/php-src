--TEST--
PEAR_Registry v1.1
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
$statedir = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'registry_tester';
if (file_exists($statedir)) {
    // don't delete existing directories!
    echo 'skip';
}
include_once 'PEAR/Registry.php';
$pv = phpversion() . '';
$av = $pv{0} == '4' ? 'apiversion' : 'apiVersion';
if (!in_array($av, get_class_methods('PEAR_Registry'))) {
    echo 'skip';
}
if (PEAR_Registry::apiVersion() != '1.1') {
    echo 'skip';
}
?>
--FILE--
<?php
error_reporting(E_ALL);
include_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'pear_registry_inc.php.inc';
include_once "PEAR/Registry.php";
PEAR::setErrorHandling(PEAR_ERROR_DIE, "%s\n");
$statedir = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'registry_tester';
cleanall();

$files1 = array(
    "pkg1-1.php" => array(
        "role" => "php",
        ),
    "pkg1-2.php" => array(
        "role" => "php",
        "baseinstalldir" => "pkg1",
        ),
    );
$files1_test = array(
    "pkg12-1.php" => array(
        "role" => "php",
        ),
    "pkg12-2.php" => array(
        "role" => "php",
        "baseinstalldir" => "pkg1",
        ),
    );
$files1_grnok = array(
    "pkg32-1.php" => array(
        "role" => "php",
        ),
    "pkg32-2.php" => array(
        "role" => "php",
        "baseinstalldir" => "pkg1",
        ),
    );
$files2 = array(
    "pkg2-1.php" => array(
        "role" => "php",
        ),
    "pkg2-2.php" => array(
        "role" => "php",
        "baseinstalldir" => "pkg2",
        ),
    );
$files3 = array(
    "pkg3-1.php" => array(
        "role" => "php",
        ),
    "pkg3-2.php" => array(
        "role" => "php",
        "baseinstalldir" => "pkg3",
        ),
    );
$files3_new = array(
    "pkg3-3.php" => array(
        "role" => "php",
        "baseinstalldir" => "pkg3",
        ),
    "pkg3-4.php" => array(
        "role" => "php",
        ),
    );

print "creating registry object\n";
$reg = new PEAR_Registry($statedir);
dumpall($reg);

$reg->addPackage("pkg1", array("name" => "pkg1", "version" => "1.0", "filelist" => $files1));
dumpall($reg);

$reg->addPackage("pkg2", array("name" => "pkg2", "version" => "2.0", "filelist" => $files2));
$reg->addPackage("pkg3", array("name" => "pkg3", "version" => "3.0", "filelist" => $files3));
dumpall($reg);

$reg->updatePackage("pkg2", array("version" => "2.1"));
dumpall($reg);

var_dump($reg->deletePackage("pkg2"));
dumpall($reg);

var_dump($reg->deletePackage("pkg2"));
dumpall($reg);

$reg->updatePackage("pkg3", array("version" => "3.1b1", "status" => "beta"));
dumpall($reg);

$testing = $reg->checkFilemap(array_merge($files3, $files2));
$ok = ($testing == array('pkg3-1.php' => 'pkg3', 'pkg3' . DIRECTORY_SEPARATOR . 'pkg3-2.php' => 'pkg3'));
echo 'filemap OK? ' . ($ok ? "yes\n" : "no\n");
if (!$ok) {
    var_dump($testing);
}

$reg->updatePackage("pkg3", array("filelist" => $files3_new));
dumpall($reg);

echo "testing channel registry\n\n";

print "test add channel\n";

require_once 'PEAR/ChannelFile.php';
$chan = new PEAR_ChannelFile;

$chan->setName('test');
$chan->setServer('pear.php.net');
$chan->setSummary('hello');
$reg->addChannel($chan);

dumpall($reg);

echo "test add bad channel\n";
$chan = new PEAR_ChannelFile;

$chan->setServer('pear.php.net');
$chan->setSummary('hello');
$reg->addChannel($chan);

dumpall($reg);

print "test add good package\n";
$reg->addPackage('pkg1', array('name' => 'pkg1', 'version' => '1.0', 'filelist' => $files1_test), 'test');

dumpall($reg);

echo "test add package with unknown channel\n";

var_dump($reg->addPackage('pkg2', array('name'=> 'pkg2', 'version' => '1.0', 'filelist' => $files1_grnok), 'grnok'));

dumpall($reg);

echo "test channel exists\n";

var_dump($reg->channelExists('test'));
var_dump($reg->channelExists('pear'));
var_dump($reg->channelExists('grnok'));

echo "testing restart with new object\n";

unset($reg);

$reg = &new PEAR_Registry($statedir);

dumpall($reg);

echo "test delete channel with packages\n";
var_dump($reg->deleteChannel('test'));

dumpall($reg);

echo "test delete channel with no packages\n";
$reg->deletePackage('pkg1', 'test');

var_dump($reg->deleteChannel('test'));

dumpall($reg);

print "tests done\n";
?>
--EXPECT--
creating registry object
dumping registry...
channel pear:
dump done
dumping registry...
channel pear:
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
dump done
dumping registry...
channel pear:
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg2: version="2.0" filelist=array(pkg2-1.php[role=php],pkg2-2.php[role=php,baseinstalldir=pkg2])
pkg3: version="3.0" filelist=array(pkg3-1.php[role=php],pkg3-2.php[role=php,baseinstalldir=pkg3])
dump done
dumping registry...
channel pear:
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg2: version="2.1" filelist=array(pkg2-1.php[role=php],pkg2-2.php[role=php,baseinstalldir=pkg2])
pkg3: version="3.0" filelist=array(pkg3-1.php[role=php],pkg3-2.php[role=php,baseinstalldir=pkg3])
dump done
bool(true)
dumping registry...
channel pear:
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg3: version="3.0" filelist=array(pkg3-1.php[role=php],pkg3-2.php[role=php,baseinstalldir=pkg3])
dump done
bool(false)
dumping registry...
channel pear:
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg3: version="3.0" filelist=array(pkg3-1.php[role=php],pkg3-2.php[role=php,baseinstalldir=pkg3])
dump done
dumping registry...
channel pear:
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg3: version="3.1b1" filelist=array(pkg3-1.php[role=php],pkg3-2.php[role=php,baseinstalldir=pkg3]) status="beta"
dump done
filemap OK? yes
dumping registry...
channel pear:
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg3: version="3.1b1" filelist=array(pkg3-3.php[role=php,baseinstalldir=pkg3],pkg3-4.php[role=php]) status="beta"
dump done
testing channel registry

test add channel
dumping registry...
channel pear:
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg3: version="3.1b1" filelist=array(pkg3-3.php[role=php,baseinstalldir=pkg3],pkg3-4.php[role=php]) status="beta"
channel test:
dump done
test add bad channel
caught ErrorStack error:
message: Missing channel name
code: 6
dumping registry...
channel pear:
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg3: version="3.1b1" filelist=array(pkg3-3.php[role=php,baseinstalldir=pkg3],pkg3-4.php[role=php]) status="beta"
channel test:
dump done
test add good package
dumping registry...
channel pear:
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg3: version="3.1b1" filelist=array(pkg3-3.php[role=php,baseinstalldir=pkg3],pkg3-4.php[role=php]) status="beta"
channel test:
pkg1: version="1.0" filelist=array(pkg12-1.php[role=php],pkg12-2.php[role=php,baseinstalldir=pkg1])
dump done
test add package with unknown channel
bool(false)
dumping registry...
channel pear:
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg3: version="3.1b1" filelist=array(pkg3-3.php[role=php,baseinstalldir=pkg3],pkg3-4.php[role=php]) status="beta"
channel test:
pkg1: version="1.0" filelist=array(pkg12-1.php[role=php],pkg12-2.php[role=php,baseinstalldir=pkg1])
dump done
test channel exists
bool(true)
bool(true)
bool(false)
testing restart with new object
dumping registry...
channel pear:
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg3: version="3.1b1" filelist=array(pkg3-3.php[role=php,baseinstalldir=pkg3],pkg3-4.php[role=php]) status="beta"
channel test:
pkg1: version="1.0" filelist=array(pkg12-1.php[role=php],pkg12-2.php[role=php,baseinstalldir=pkg1])
dump done
test delete channel with packages
bool(false)
dumping registry...
channel pear:
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg3: version="3.1b1" filelist=array(pkg3-3.php[role=php,baseinstalldir=pkg3],pkg3-4.php[role=php]) status="beta"
channel test:
pkg1: version="1.0" filelist=array(pkg12-1.php[role=php],pkg12-2.php[role=php,baseinstalldir=pkg1])
dump done
test delete channel with no packages
bool(true)
dumping registry...
channel pear:
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg3: version="3.1b1" filelist=array(pkg3-3.php[role=php,baseinstalldir=pkg3],pkg3-4.php[role=php]) status="beta"
dump done
tests done
