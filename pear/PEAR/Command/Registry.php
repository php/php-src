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
    function PEAR_Command_Registry(&$ui, &$config)
    {
        parent::PEAR_Command_Common($ui, $config);
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

    function getHelp($command)
    {
        switch ($command) {
            case 'list-installed':
                return array(null, 'List the installed PEAR packages in the system');
        }
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
                $this->ui->startTable(
                    array('caption' => 'Installed packages:',
                          'border' => true));
                foreach ($installed as $package) {
                    if ($i++ % 20 == 0) {
                        $this->ui->tableRow(array('Package', 'Version', 'State'),
                                        array('bold' => true));
                    }
                    $this->ui->tableRow(array($package['package'],
                                              $package['version'],
                                              $package['release_state']));
                }
                $this->ui->endTable();
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