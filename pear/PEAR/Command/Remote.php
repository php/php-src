<?php

require_once 'PEAR/Command/Common.php';
require_once 'PEAR/Remote.php';

class PEAR_Command_Remote extends PEAR_Command_Common
{
    // {{{ constructor

    /**
     * PEAR_Command_Remote constructor.
     *
     * @access public
     */
    function PEAR_Command_Remote(&$ui, &$config)
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
        return array('remote-package-info',
                     'list-upgrades',
                     'list-remote-packages');
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
        $remote = &new PEAR_Remote($this->config);
        switch ($command) {
            case 'remote-package-info': {
                break;
            }
            case 'list-remote-packages': {
                break;
            }
            case 'list-upgrades': {
                include_once "PEAR/Registry.php";
                if (empty($params[0])) {
                    $state = $this->config->get('preferred_state');
                } else {
                    $state = $params[0];
                }
                $caption = 'Available Upgrades';
                if (empty($state) || $state == 'any') {
                    $latest = $remote->call("package.listLatestReleases");
                } else {
                    $latest = $remote->call("package.listLatestReleases", $state);
                    $caption .= ' (' . $state . ')';
                }
                $caption .= ':';
                if (PEAR::isError($latest)) {
                    return $latest;
                }
                $reg = new PEAR_Registry($this->config->get('php_dir'));
                $inst = array_flip($reg->listPackages());
                $this->ui->startTable(array('caption' => $caption,
                                            'border' => 1));
                $this->ui->tableRow(array('Package', 'Version', 'Size'),
                                array('bold' => true));
                foreach ($latest as $package => $info) {
                    if (!isset($inst[$package])) {
                        // skip packages we don't have installed
                        continue;
                    }
                    extract($info);
                    $inst_version = $reg->packageInfo($package, 'version');
                    if (version_compare($version, $inst_version, "le")) {
                        // installed version is up-to-date
                        continue;
                    }
                    if ($filesize >= 20480) {
                        $filesize += 1024 - ($filesize % 1024);
                        $fs = sprintf("%dkB", $filesize / 1024);
                    } elseif ($filesize > 0) {
                        $filesize += 103 - ($filesize % 103);
                        $fs = sprintf("%.1fkB", $filesize / 1024.0);
                    } else {
                        $fs = "  -"; // XXX center instead
                    }
                    $this->ui->tableRow(array($package, $version, $fs));
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