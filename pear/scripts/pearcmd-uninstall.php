<?php

include_once 'PEAR/Installer.php';
$pkgfile = $cmdargs[0];
$installer =& new PEAR_Installer($script_dir, $ext_dir, $doc_dir);
$installer->setErrorHandling(PEAR_ERROR_DIE,
                             basename($pkgfile) . ": %s\n");
$installer->debug = $verbose;
$uninstall_options = array();
foreach ($cmdopts as $opt) {
    switch ($opt[0]) {
        case 'r':
            $uninstall_options['register_only'] = true;
            break;
        case 'f':
            $uninstall_options['force'] = true;
            break;
    }
}
if ($installer->uninstall($pkgfile, $uninstall_options)) {
    print "uninstall ok\n";
} else {
    print "uninstall failed\n";
}

?>
