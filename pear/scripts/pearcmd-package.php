<?php

include_once 'PEAR/Packager.php';
$pkginfofile = isset($cmdargs[0]) ? $cmdargs[0] : null;
$packager =& new PEAR_Packager($script_dir, $ext_dir, $doc_dir);
$packager->setErrorHandling(PEAR_ERROR_DIE, "pear page: %s\n");
$packager->debug = $verbose;
if (PEAR::isError($packager->Package($pkginfofile))) {
    print "\npackage failed\n";
} else {
    print "package ok\n";
}


?>