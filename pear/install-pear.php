<?php

$pear_dir = dirname(__FILE__);
ini_set('include_path', $pear_dir);
##//include_once 'PEAR/Config.php';
include_once 'PEAR.php';
include_once 'PEAR/Installer.php';
include_once 'PEAR/Registry.php';
include_once 'PEAR/Frontend/CLI.php';

##//$config = &PEAR_Config::singleton();

array_shift($argv);
if ($argv[0] == '--force') {
    array_shift($argv);
    $force = true;
} else {
    $force = false;
}
// package => install_file
$install_files = array();

/*
$dp = opendir($pear_dir);
while ($ent = readdir($dp)) {
    if (ereg('^package-(.*)\.xml$', $ent, $matches)) {
        $install_files[$matches[1]] = $ent;
    }
}
closedir($dp);
*/
foreach ($argv as $arg) {
    $bn = basename($arg);
    if (ereg('^package-(.*)\.xml$', $bn, $matches) ||
        ereg('^([A-Za-z0-9_:]+)-.*\.(tar|tgz)$', $bn, $matches)) {
        $install_files[$matches[1]] = $arg;
    }
}

$config = &PEAR_Config::singleton();

// make sure we use only default values
$config_layers = $config->getLayers();
foreach ($config_layers as $layer) {
    if ($layer == 'default') continue;
    $config->removeLayer($layer);
}
$config->set('verbose', 0, 'default');

$reg = &new PEAR_Registry($config->get('php_dir'));
$ui = &new PEAR_Frontend_CLI();
$installer = &new PEAR_Installer($ui);

foreach ($install_files as $package => $instfile) {
    if ($reg->packageExists($package)) {
        $info = $installer->infoFromAny($instfile);
        if (PEAR::isError($info)) {
            $ui->outputData(sprintf("[PEAR] %s: %s", $package, $info->getMessage()));
            continue;
        }
        $new_ver = $info['version'];
        $old_ver = $reg->packageInfo($package, 'version');
        if (version_compare($new_ver, $old_ver, 'gt')) {
            $err = $installer->install($instfile, array('upgrade' => true));
            if (PEAR::isError($err)) {
                $ui->outputData(sprintf("[PEAR] %s: %s", $package, $err->getMessage()));
                continue;
            }
            $ui->outputData(sprintf("[PEAR] %-15s- upgraded:  %s", $package, $new_ver));
        } else {
            if (@$argv[1] == '--force') {
                $err = $installer->install($instfile, array('force' => true));
                if (PEAR::isError($err)) {
                    $ui->outputData(sprintf("[PEAR] %s: %s", $package, $err->getMessage()));
                    continue;
                }
                $ui->outputData(sprintf("[PEAR] %-15s- installed: %s", $package, $new_ver));
            } else {
                $ui->outputData(sprintf("[PEAR] %-15s- already installed: %s", $package, $old_ver));
            }
        }
    } else {
        $err = $installer->install($instfile, array('nodeps' => true));
        if (PEAR::isError($err)) {
            $ui->outputData(sprintf("[PEAR] %s: %s", $package, $err->getMessage()));
            continue;
        }
        $new_ver = $reg->packageInfo($package, 'version');
        $ui->outputData(sprintf("[PEAR] %-15s- installed: %s", $package, $new_ver));
    }
}

?>