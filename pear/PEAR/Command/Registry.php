<?php

require_once 'PEAR/Command/Common.php';
require_once 'PEAR/Registry.php';
require_once 'PEAR/Config.php';

class PEAR_Command_Registry extends PEAR_Command_Common
{
    // {{{ constructor

    /**
     * PEAR_Command_Registry constructor.
     *
     * @access public
     */
    function PEAR_Command_Registry(&$ui)
    {
        parent::PEAR_Command_Common($ui);
    }

    // }}}

    // {{{ getCommands()

    /**
     * Return a list of all the commands defined by this class.
     * @return array list of commands
     * @access public
     */
    function getCommands()
    {
        return array('list-installed');
    }

    // }}}
    // {{{ run()

    /**
     * Execute the command.
     *
     * @param string command name
     *
     * @param array option_name => value
     *
     * @param array list of additional parameters
     *
     * @return bool TRUE on success, FALSE if the command was unknown,
     *              or a PEAR error on failure
     *
     * @access public
     */
    function run($command, $options, $params)
    {
        $failmsg = '';
        $cf = &PEAR_Config::singleton();
        switch ($command) {
            case 'list-installed': {
                $reg = new PEAR_Registry($cf->get('php_dir'));
                $installed = $reg->packageInfo();
                $i = $j = 0;
                heading("Installed packages:");
                foreach ($installed as $package) {
                    if ($i++ % 20 == 0) {
                        if ($j++ > 0) {
                            print "\n";
                        }
                        printf("%-20s %-10s %s\n",
                               "Package", "Version", "State");
                        print str_repeat("-", 75)."\n";
                    }
                    printf("%-20s %-10s %s\n", $package['package'],
                           $package['version'], $package['release_state']);
                }
                break;
            }
            default: {
                return false;
            }
        }
        if ($failmsg) {
            return $this->raiseError($failmsg);
        }
        return true;
    }

    // }}}


}

?>