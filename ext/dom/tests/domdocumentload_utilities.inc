<?php

function libxml_options_to_int($libxmlOptions) {

    $defined_constants = get_defined_constants(true);
    $env_array = explode('|', $libxmlOptions);
    $libxml_constants = array_intersect_key($defined_constants['libxml'], array_flip($env_array));

    $sum = 0;
    foreach($libxml_constants as $value) {
        $sum = $sum|$value;
    }

    return $sum;
}
?>
