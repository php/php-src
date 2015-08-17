--TEST--
proto bool enchant_broker_set_dict_path(resource broker, int dict_type, string value) function
proto string enchant_broker_get_dict_path(resource broker, int dict_type) function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php
if(!extension_loaded('enchant')) die('skip, enchant not loader');
if (!is_resource(enchant_broker_init())) {die("skip, resource dont load\n");}
if (!is_array(enchant_broker_list_dicts(enchant_broker_init()))) {die("skip, dont has dictionary install in this machine! \n");}
?>
--FILE--
<?php
$broker = enchant_broker_init();
$windowsOS = "c:\windows\is\a\bad\OS";
$linuxOS = "/home/fedora/is/the/better/distro/linux";
$dictTipeWin = 1;
$dictTipeLinux = 2;

if (is_resource($broker)) {
    echo("OK\n");
    
    if (enchant_broker_set_dict_path($broker, $dictTipeWin, $windowsOS)) {
        echo("OK\n");
        
        if (enchant_broker_set_dict_path($broker, $dictTipeLinux, $linuxOS)) {
            echo("OK\n");
                
            if ( 
                  (enchant_broker_get_dict_path($broker,$dictTipeWin) == $windowsOS) && 
                  (enchant_broker_get_dict_path($broker,$dictTipeLinux) == $linuxOS)            
              ) {
                   echo("OK\n");

            } else {
                   echo("broker get dict path has failed \n");
            }                    
        
        } else {
           echo("broker set dict path {$linuxOS} has failed \n");
        }
    } else {
        echo("broker set dict path {$windows} has failed \n");
    }
} else {
    echo("broker is not a resource; failed; \n");
}
?>
--EXPECT--
OK
OK
OK
OK
