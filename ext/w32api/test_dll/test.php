<?php

        dl("php_w32api.dll");
        
        w32api_register_function("test_dll.dll", "print_names", "long");
        w32api_deftype("name", "string", "fname", "string", "lname");
        $rsrc = w32api_init_dtype("name", "James", "Moore");
        print_names($rsrc);

?>
