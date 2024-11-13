<?php
function skipif(bool $want_expat) {
    ob_start();
    echo phpinfo(INFO_MODULES);
    $output = ob_get_clean();
    if ($want_expat !== str_contains($output, 'EXPAT')) {
        die('skip test is for ' . ($want_expat ? 'Expat' : 'libxml'));
    }
}
