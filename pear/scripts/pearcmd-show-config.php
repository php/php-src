<?php

$keys = $config->getKeys();
foreach ($keys as $key) {
    $value = $config->get($key);
    $xi = "";
    if ($config->isDefaulted($key)) {
        $xi .= " (default)";
    }
    if (isset($fallback_done[$key])) {
        $xi .= " (built-in)";
    }
    printf("%s = %s%s\n", $key, $value, $xi);
}

?>