<?php

require_once 'PEAR/Command/Common.php';
require_once 'PEAR/Packager.php';

class PEAR_Command_Package extends PEAR_Command_Common
{
    // {{{ constructor

    /**
     * PEAR_Command_Login constructor.
     *
     * @access public
     */
    function PEAR_Command_Package(&$ui, &$config)
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
        return array('package');
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
     * @return bool TRUE on success, FALSE for unknown commands, or
     * a PEAR error on failure
     *
     * @access public
     */
    function run($command, $options, $params)
    {
        $failmsg = '';
        switch ($command) {
            case 'package': {
                $pkginfofile = isset($params[0]) ? $params[0] : null;
                ob_start();
                $packager =& new PEAR_Packager($this->config->get('php_dir'),
                                               $this->config->get('ext_dir'),
                                               $this->config->get('doc_dir'));
                $packager->debug = $this->config->get('verbose');
                $result = $packager->Package($pkginfofile);
                $output = ob_get_contents();
                ob_end_clean();
                $lines = explode("\n", $output);
                foreach ($lines as $line) {
                    $this->ui->displayLine($line);
                }
                if (PEAR::isError($result)) {
                    $this->ui->displayLine("Package failed!");
                } else {
                    $this->ui->displayLine("Package ok.");
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