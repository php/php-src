<?php
function xml_uses_expat(): bool {
    ob_start();
    echo phpinfo(INFO_MODULES);
    return str_contains(ob_get_clean(), 'EXPAT');
}
