--TEST--
PEAR_Common::sortPkgDeps test
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
?>
--FILE--
<?php
$dir = getcwd();
chdir(dirname(__FILE__));

require_once 'PEAR/Common.php';

$c = new PEAR_Common();

$packages = array(
'common_sortPkgDeps6_package.xml',
'common_sortPkgDeps2_package.xml',
'common_sortPkgDeps1_package.xml',
'common_sortPkgDeps4_package.xml',
'common_sortPkgDeps5_package.xml',
'common_sortPkgDeps3_package.xml',
);

$uninstallpackages = array(
$c->infoFromAny('common_sortPkgDeps6_package.xml'),
$c->infoFromAny('common_sortPkgDeps2_package.xml'),
$c->infoFromAny('common_sortPkgDeps1_package.xml'),
$c->infoFromAny('common_sortPkgDeps4_package.xml'),
$c->infoFromAny('common_sortPkgDeps5_package.xml'),
$c->infoFromAny('common_sortPkgDeps3_package.xml'),
);

echo "Test Install Sort:\n";
$c->sortPkgDeps($packages);
dumpPacks($packages);

echo "Test Uninstall Sort:\n";
$c->sortPkgDeps($uninstallpackages, true);
dumpPacks($uninstallpackages);

chdir($dir);

function dumpPacks($p)
{
    echo "Packages(\n";
    foreach ($p as $inf) {
        echo $inf['info']['package'] . ",\n";
    }
    echo ")\n";
}
?>
--GET--
--POST--
--EXPECT--
Test Install Sort:
Packages(
pkg6,
pkg5,
pkg4,
pkg3,
pkg2,
pkg1,
)
Test Uninstall Sort:
Packages(
pkg1,
pkg2,
pkg3,
pkg5,
pkg4,
pkg6,
)